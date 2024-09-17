__all__ = [
    "convert_i3s_to_c"
]

from . import (
    c_ast,
    c_generator
)

from copy import copy

binary_op = {
    # op: (func_name_base, support int param, commutativity)
    '+'     : ('tcg_gen_add', True, True),
    '-'     : ('tcg_gen_sub', True, False),
    '*'     : ('tcg_gen_mul', True, True),
    '^'     : ('tcg_gen_xor', True, True),
    '|'     : ('tcg_gen_or', True, True),
    '&'     : ('tcg_gen_and', True, True),
    '>>'    : (('tcg_gen_shr', 'tcg_gen_sar'), True, False),
    '<<'    : ('tcg_gen_shl', True, False),
    '>>>'   : ('tcg_gen_rotr', True, False),
    '<<<'   : ('tcg_gen_rotl', True, False),
    '/'     : ('tcg_gen_div', False, False),
    '%'     : ('tcg_gen_rem', False, False)
}


binary_logical_op = ['||', '&&', '==', '!=', '>=', '<=', '>', '<']


def get_tcg_type(node):
    if isinstance(node, c_ast.ArrayRef):
        # TODO: support tcg index
        if get_tcg_type(node.subscript):
            raise TypeError(
                '{}: unsupported array index with TCG type'.format(node.coord)
            )
        return get_tcg_type(node.name)
    elif isinstance(node, c_ast.ID):
        # Some ID, such macro, has no type
        vt = node.var_type
        if vt is not None:
            vt = set(vt)
            if ('tcg' in vt
             or 'TCGv' in vt
             or 'TCGv_i64' in vt
             or 'TCG_i32' in vt
            ):
                return vt

    return None


def set_tcg_type(node, node_type):
    if isinstance(node, c_ast.ArrayRef):
        set_tcg_type(node.name, node_type)
    elif isinstance(node, c_ast.ID):
        node.var_type = node_type
    else:
        raise TypeError(
            "{}: can't set tcg type to {}".format(
                node.coord, node.__class__.__name__
            )
        )


def get_tcg_suffix(identifier_type):
    if not identifier_type:
        return None

    if 'long' in identifier_type:
        suffix = '_i64'
    elif 'short' in identifier_type:
        suffix = '_i32'
    else:
        suffix = '_tl'

    return suffix


def get_node_desc(node):
    '''return (type, suffix)'''
    node_type = get_tcg_type(node)
    node_suffix = get_tcg_suffix(node_type)
    return (node_type, node_suffix)


def get_id(node):
    res = node
    while isinstance(res, (c_ast.ArrayRef, c_ast.StructRef)):
        res = res.name
    return res


def get_indent(child):
    s = ''
    for c in child.prefix:
        if c == ' ' or c == '\t':
            s += c
        else:
            s = ''
    return '\n' + s


def fixup_indent(node, indent):
    '''Node prefix and node's chilren prefix will be converted according
    following rule: if prefix contains indent as substring all subsequent
    spaces and tabs will be ignored
    '''

    def cut_indent(prefix, indent):
        new_prefix = ''
        i = 0
        indent_len = len(indent)
        for c in prefix:
            if i < indent_len:
                new_prefix += c
                if c == indent[i]:
                    i += 1
                elif c == indent[0]:
                    i = 1
                else:
                    i = 0
            elif i == indent_len:
                if c == ' ' or c == '\t':
                    continue
                else:
                    new_prefix += c
                    i = 0
            else:
                new_prefix += c
                i = 0
        return new_prefix

    def bypass_children(cur_node, indent):
        if isinstance(cur_node, c_ast.Compound):
            # Compound's prefix is a list with 2 elements
            cur_node.prefix[0] = cut_indent(cur_node.prefix[0], indent)
            cur_node.prefix[1] = cut_indent(cur_node.prefix[1], indent)
            if cur_node.block_items is not None:
                indent += '    '
        else:
            cur_node.prefix = cut_indent(cur_node.prefix, indent)

        for __, child in cur_node.children():
            bypass_children(child, indent)

    bypass_children(node, indent)

# nodes with immediate prefix
nip = (
    c_ast.ID,
    c_ast.Constant,
    c_ast.Paren,
    c_ast.UnaryOp,
    c_ast.For,
    c_ast.While,
    c_ast.If,
    c_ast.DoWhile
)


def set_node_prefix(node, prefix):
    if isinstance(node, c_ast.ArrayRef):
        return set_node_prefix(node.name, prefix)
    elif isinstance(node, nip):
        node.prefix = prefix
        return True
    elif isinstance(node, c_ast.Decl):
        if node.quals:
            node.prefix = prefix
            return True

        node_type = node.type
        while isinstance(node_type, c_ast.PtrDecl):
            node_type = node_type.type

        if isinstance(node_type.type, c_ast.IdentifierType):
            node_type.type.prefix = prefix
            return True

    for __, child in node.children():
        if set_node_prefix(child, prefix):
            break

    return False


def get_node_prefix(node):
    if isinstance(node, c_ast.ArrayRef):
        return get_node_prefix(node.name)
    elif isinstance(node, nip):
        return node.prefix
    elif isinstance(node, c_ast.Decl):
        if node.quals:
            return node.prefix
        node_type = node.type
        while isinstance(node_type, c_ast.PtrDecl):
            node_type = node_type.type

        if isinstance(node_type.type, c_ast.IdentifierType):
            return node_type.type.prefix

    for __, child in node.children():
        prefix = get_node_prefix(child)
        if prefix != '':
            return prefix

    return ''


def gen_terminator_label(parent, indent, label_name):
    set_label = c_ast.ID('gen_set_label')
    if isinstance(parent, c_ast.Compound):
        # save brace close prefix
        set_label.prefix = parent.prefix[1]
        fixup_indent(set_label, indent)
    else:
        set_label.prefix = indent

    return c_ast.FuncCall(
        set_label,
        c_ast.ExprList([c_ast.ID(label_name)])
    )


# ArrayRef, ID and Constant are simple node
def duplicate_simple_node(node):
    if isinstance(node, c_ast.ID):
        return c_ast.ID(node.name,
            coord = node.coord,
            prefix = node.prefix,
            var_type = list(node.var_type)
        )
    elif isinstance(node, c_ast.Constant):
        return c_ast.Constant(node.type, node.value,
            coord = node.coord,
            prefix = node.prefix
        )
    elif isinstance(node, c_ast.ArrayRef):
        # TODO: fixup subscript duplicate
        return c_ast.ArrayRef(
            duplicate_simple_node(node.name),
            node.subscript,
            coord = node.coord,
            prefix = node.prefix
        )
    else:
        raise ValueError(
            "{}: can't duplicate {}".format(
                node.coord,
                node.__class__.__name__
            )
        )


def change_declname(node, new_name):
    for _, child in node.children():
        change_declname(child, new_name)

    if isinstance(node, c_ast.TypeDecl):
        node.declname = new_name


class CompoundState(object):
    def __init__(self, prev_state = None):

        self.vars = set()
        # names to change
        self.n2c = {}
        self.tcg_compound = False

        self.tcg_list = []
        self.tcg_local_list = []
        self.tcg_array_list = []
        self.tcg_local_array_list = []
        self.indent = None
        self.prev_state = prev_state

        self.tcg_tmp_list = []
        self.tcg_tmp_local_list = []
        # Shadow 'tcg_tmp_list' list consisting of flags
        # False flag means var is free to reuse
        self.tcg_tmp_list_hold = []
        self.tcg_tmp_local_list_hold = []
        self.tcg_tmp_count = 0
        self.tcg_tmp_local_count = 0
        self.tcg_label_count = 0
        self.subast = []

    def tcg_label_decl(self):
        name = 'i3s_label_' + str(self.tcg_label_count)
        self.tcg_label_count += 1

        type_decl = c_ast.PtrDecl(
            [],
            c_ast.TypeDecl(name, None,
                c_ast.IdentifierType(['TCGLabel'], prefix = self.indent)
            ),
            prefix = ' '
        )

        self.subast.append(
            c_ast.Decl(
                name = name,
                quals = [],
                storage = [],
                funcspec = [],
                type = type_decl,
                init = c_ast.FuncCall(
                    c_ast.ID('gen_new_label', prefix = ' '),
                    None
                ),
                bitsize = None
            )
        )

        return name

    def insert_tcg_free(self, local_free = False, do_clean = True, cs = None):
        '''Append 'tcg_temp_free' to current CS subast (self.cs.subast)
        for every temp var (with local temp var if 'local_free' is True)
        in 'cs' to current CS subast (self.cs.subast).
        If 'do_clean' is True lists with temp will be clean.
        '''
        if cs is None:
            cs = self

        free_list = cs.tcg_tmp_list + cs.tcg_list
        if do_clean is True:
            cs.tcg_tmp_list = []
            cs.tcg_list = []
            cs.tcg_tmp_list_hold = []
        if local_free is True:
            free_list += cs.tcg_local_list + cs.tcg_tmp_local_list
            if do_clean is True:
                cs.tcg_local_list = []
                cs.tcg_tmp_local_list = []
                cs.tcg_tmp_local_list_hold = []

        for v in free_list:
            self.subast.append(c_ast.FuncCall(
                c_ast.ID('tcg_temp_free', prefix = self.indent),
                c_ast.ExprList([c_ast.ID(v)])
            ))

        self.subast.extend(cs.tcg_array_list)
        if local_free is True:
            self.subast.extend(cs.tcg_local_array_list)

    def tcg_temp_decl(self, temp_type, local = False):
        suffix = get_tcg_suffix(temp_type)

        if local:
            name = 'i3s_t' + str(self.tcg_tmp_local_count) + '_local' + suffix
            init_func = 'tcg_temp_local_new'
            self.tcg_tmp_local_list.append(name)
            self.tcg_tmp_local_list_hold.append(True)
            self.tcg_tmp_local_count += 1
        else:
            init_func = 'tcg_temp_new'
            name = 'i3s_t' + str(self.tcg_tmp_count) + suffix
            self.tcg_tmp_list.append(name)
            self.tcg_tmp_list_hold.append(True)
            self.tcg_tmp_count += 1

        type_name = 'TCGv'
        if suffix != '_tl':
            type_name += suffix
            init_func += suffix

        type_decl = c_ast.TypeDecl(name, None,
            c_ast.IdentifierType([type_name], prefix = self.indent),
            prefix = ' '
        )

        self.subast.append(c_ast.Decl(
            name = name,
            quals = [],
            storage = [],
            funcspec = [],
            type = type_decl,
            init = c_ast.FuncCall(c_ast.ID(init_func, prefix = ' '), None),
            bitsize = None
        ))

        return c_ast.ID(name, var_type = temp_type)

    def get_unoccupied_tmp_tcg(self, temp_type, local = False):
        suffix = get_tcg_suffix(temp_type)
        if local:
            hold_list = self.tcg_tmp_local_list_hold
            tmp_list = self.tcg_tmp_local_list
        else:
            hold_list = self.tcg_tmp_list_hold
            tmp_list = self.tcg_tmp_list

        for i, flag in enumerate(hold_list):
            if flag is False and tmp_list[i].endswith(suffix):
                hold_list[i] = True
                return c_ast.ID(tmp_list[i], var_type = temp_type)
        else:
            return self.tcg_temp_decl(temp_type, local)

    def convert_var_to_tcg(self, val, res_type):
        suffix = get_tcg_suffix(res_type)
        tmp = self.get_unoccupied_tmp_tcg(suffix)

        if isinstance(val, c_ast.Paren):
            val = val.expr
        set_node_prefix(val, ' ')

        self.subast.append(c_ast.FuncCall(
            c_ast.ID('tcg_gen_movi' + suffix),
            c_ast.ExprList([tmp, val])
        ))
        # create new insteance for next FuncCall
        return c_ast.ID(tmp.name, var_type = res_type)

    def cast(self, src_desc, dest_desc, local = False):
        '''desc = (node, type, suffix)'''

        src, src_type, src_suffix = src_desc
        dest, dest_type, dest_suffix = dest_desc

        if src_suffix is None:
            if src_type is None:
                src_suffix = get_tcg_suffix(get_tcg_type(src))
            else:
                src_suffix = get_tcg_suffix(src_type)

        if dest_type is None:
            dest_type = get_tcg_type(dest)

        if dest_suffix is None:
            dest_suffix = get_tcg_suffix(dest_type)

        if src_suffix == dest_suffix:
            if src_suffix is not None:
                set_tcg_type(src, dest_type)
            return src
        elif dest_suffix is None and src_suffix is not None:
            raise TypeError(
                "{}: cast TCG var to non-TCG var".format(src.coord)
            )

        if src_suffix is None:
            func_name = 'tcg_gen_movi' + dest_suffix
        elif ( src_suffix == '_i32'
            or src_suffix == '_tl' and dest_suffix == '_i64'
        ):
            func_name = 'tcg_gen_ext' \
                + ('' if 'signed' in dest_type else 'u') \
                + src_suffix + dest_suffix
        else:
            func_name = 'tcg_gen_trunc' + src_suffix + dest_suffix

        set_node_prefix(src, ' ')
        if dest is not None:
            set_node_prefix(dest, '')
            self.subast.append(c_ast.FuncCall(
                c_ast.ID(func_name),
                c_ast.ExprList([dest, src])
            ))
            return dest
        else:
            res_var = self.get_unoccupied_tmp_tcg(dest_type, local)
            self.subast.append(c_ast.FuncCall(
                c_ast.ID(func_name),
                c_ast.ExprList([res_var, src])
            ))
            # create new insteance for next FuncCall
            return c_ast.ID(res_var.name, var_type = dest_type)

    def get_bin_expr_param(self, rv, lv, is_commutative, support_int_rv,
            r_shift = False
    ):
        rv_type, rv_suffix = get_node_desc(rv)
        lv_type, lv_suffix = get_node_desc(lv)
        i_suffix = ''

        if rv_suffix is None:
            # rv - non TCG
            if lv_suffix is None:
                # lv - non TCG
                return None
            else:
                # lv - TCG
                p1 = lv
                suffix = lv_suffix
                res_type = lv_type
                if support_int_rv:
                    # tcg func support int rv
                    i_suffix += 'i'
                    p2 = rv
                else:
                    # tcg func don't support non TCG rv
                    p2 = self.convert_var_to_tcg(rv, res_type)
        else:
            # rv - TCG
            if lv_suffix is None:
                # lv - non TCG
                suffix = rv_suffix
                res_type = rv_type
                if is_commutative and support_int_rv:
                    i_suffix = 'i'
                    p1 = rv
                    p2 = lv
                else:
                    # tcg func don't support non TCG lv
                    p1 = self.convert_var_to_tcg(lv, res_type)
                    p2 = rv
            elif lv_suffix == rv_suffix:
                # lv - TCG
                suffix = lv_suffix
                if r_shift:
                    res_type = lv_type
                else:
                    # if one of the parameter is unsigned the result is
                    # unsigned too
                    if 'signed' in rv_type:
                        res_type = lv_type
                    else:
                        res_type = rv_type

                p1 = lv
                p2 = rv
            else:
                if r_shift:
                    res_type = lv_type
                    suffix = lv_suffix
                    p1 = lv
                    p2 = self.cast(
                        (rv, rv_type, rv_suffix),
                        (None, res_type, suffix)
                    )
                else:
                    if 'long' in lv_type:
                        if 'short' in rv_type:
                            res_type = lv_type
                            suffix = lv_suffix
                            p1 = lv
                            p2 = self.cast(
                                (rv, rv_type, rv_suffix),
                                (None, res_type, suffix)
                            )
                        else:
                            # rv - tl
                            res_type = rv_type
                            suffix = rv_suffix
                            p1 = self.cast(
                                (lv, lv_type, lv_suffix),
                                (None, res_type, suffix)
                            )
                            p2 = rv
                    elif 'short' in lv_type:
                        res_type = rv_type
                        suffix = rv_suffix
                        p1 = self.cast(
                            (lv, lv_type, lv_suffix),
                            (None, res_type, suffix)
                        )
                        p2 = rv
                    else:
                        # lv - tl
                        res_type = lv_type
                        suffix = lv_suffix
                        p1 = lv
                        p2 = self.cast(
                            (rv, rv_type, rv_suffix),
                            (None, res_type, suffix)
                        )

        return (res_type, suffix, i_suffix, p1, p2)

# debug comment prefix
DCP = '/* src: '
# debug comment suffix
DCS = ' */'


def insert_debug_info(prefix, comment):
    # Insert debug info keeping node's indent and comment.
    # Debug info should be right above the node.
    # AST depth-first traversaled so we insert debug comment
    # above previous debug comments.
    dcp_idx = prefix.find(DCP)
    idx = prefix.rfind('\n', 0, dcp_idx)
    if idx == -1:
        return prefix + comment
    else:
        return prefix[:idx] + comment + prefix[idx:]


class I3SProcessing(object):
    def __init__(self):
        self.cs = None
        self.break_label = []
        # 'continue_label' contains list [label name, is used] or None
        # flag 'is used' is needed to avoid extra label generation
        self.continue_label = []

        self.generator = c_generator.CGenerator()

    def processing(self, node, parent, debug = False):
        if node is not None:
            method = 'processing_' + node.__class__.__name__
            return getattr(self, method, self.generic_processing)(
                node, parent, debug
            )

    def generic_processing(self, node, parent, debug):
        for __, child in node.children():
            self.processing(child, node, debug)

    def processing_ArrayRef(self, node, parent, debug):
        return node

    def processing_StructRef(self, node, parent, debug):
        return node

    def processing_ID(self, node, parent, debug):
        cs = self.cs
        if cs.tcg_compound and node.name in cs.n2c:
            new_name = cs.n2c[node.name]
            print('Attention: {}: rename "{}" --> "{}"'.format(
                node.coord, node.name, new_name)
            )
            node.name = new_name
        return node

    def processing_Constant(self, node, parent, debug):
        return node

    def processing_Continue(self, node, parent, debug):
        cs = self.cs
        if cs.indent is None:
            cs.indent = get_indent(node)
        if self.continue_label[-1] is None:
            cs.insert_tcg_free(True)
            if cs.subast:
                cs.subast.append(node)
        else:
            cs.insert_tcg_free()
            if debug:
                prefix = insert_debug_info(
                    node.prefix,
                    cs.indent + DCP + 'continue' + DCS
                )
            else:
                prefix = node.prefix
            cs.subast.append(c_ast.FuncCall(
                c_ast.ID('tcg_gen_br', prefix = prefix),
                c_ast.ExprList([c_ast.ID(self.continue_label[-1][0])])
            ))
            self.continue_label[-1][1] = True

    def processing_Break(self, node, parent, debug):
        cs = self.cs
        if cs.indent is None:
            cs.indent = get_indent(node)
        if self.break_label[-1] is None:
            cs.insert_tcg_free(True)
            if cs.subast:
                cs.subast.append(node)
        else:
            cs.insert_tcg_free()
            if debug:
                prefix = insert_debug_info(
                    node.prefix,
                    cs.indent + DCP + 'break' + DCS
                )
            else:
                prefix = node.prefix
            cs.subast.append(c_ast.FuncCall(
                c_ast.ID('tcg_gen_br', prefix = prefix),
                c_ast.ExprList([c_ast.ID(self.break_label[-1])])
            ))

    def processing_Return(self, node, parent, debug):
        # Attention: Don't return tcg variable or expression!
        cs = self.cs
        if cs.indent is None:
            cs.indent = get_indent(node)
        cur_compound = True
        while cs is not None:
            if cur_compound:
                cs.insert_tcg_free(True, True)
                cur_compound = False
            else:
                self.cs.insert_tcg_free(True, False, cs)
            cs = cs.prev_state

        self.cs.subast.append(node)

    def processing_FuncCall(self, node, parent, debug):
        cs = self.cs
        if cs.indent is None:
            indent = get_indent(node.name)
            cs.indent = indent

        if node.args_type is None:
            return node

        args_count = len(node.args_type)

        if args_count == 1 and node.args_type[0][0] == "void":
            return node

        old_local = copy(cs.tcg_tmp_local_list_hold)

        given_args = len(node.args.exprs)

        if args_count != given_args:
            raise TypeError(
                '{}: function has {} args, but {} args were given'.format(
                    node.coord, args_count, given_args
                )
            )

        zip_arg_with_type = zip(node.args.exprs, node.args_type)
        has_tcg_arg = False

        if debug:
            p = node.name.prefix
            node.name.prefix = ''
            c = cs.indent + DCP + self.generator.visit(node) + DCS
            node.name.prefix = p
            prefix = insert_debug_info(node.name.prefix, c)
        else:
            prefix = node.name.prefix

        for i, (arg, arg_type) in enumerate(zip_arg_with_type):
            at = set(arg_type)
            at_tcg_inter = at.intersection(
                ('tcg', 'TCGv', 'TCGv_i64', 'TCG_i32')
            )

            if at_tcg_inter:
                has_tcg_arg = True

                p_arg = self.processing(arg, node, debug)
                if isinstance(arg, c_ast.FuncCall):
                    res = arg
                else:
                    res = cs.cast(
                        (p_arg, None, None),
                        (None, at, None),
                        True
                    )
                    for e in cs.subast:
                        set_node_prefix(e, cs.indent)

                if res.name in cs.tcg_tmp_list:
                    local_var = cs.get_unoccupied_tmp_tcg(at, True)
                    suffix = get_tcg_suffix(at)
                    res.prefix = ' '
                    self.cs.subast.append(c_ast.FuncCall(
                        c_ast.ID('tcg_gen_mov' + suffix, prefix = cs.indent),
                        c_ast.ExprList([
                            local_var,
                            res
                        ]),
                    ))
                    res = duplicate_simple_node(local_var)

                if arg is not res:
                    if i > 0:
                        res.prefix = ' '
                    node.args.exprs[i] = res
            else:
                if get_tcg_type(arg) is not None:
                    raise TypeError(
                        "{}: can't cast TCG var to non-TCG type".format(
                            node.coord
                        )
                    )

        if not has_tcg_arg:
            return node

        len_hold = len(old_local)
        for i in range(len(cs.tcg_tmp_local_list_hold)):
            if i < len_hold:
                cs.tcg_tmp_local_list_hold[i] = old_local[i]
            else:
                cs.tcg_tmp_local_list_hold[i] = False

        cs.insert_tcg_free()

        if isinstance(parent,
            (c_ast.Assignment, c_ast.BinaryOp, c_ast.FuncCall)
        ):
            # function call will be added by parent
            return

        if cs.subast:
            set_node_prefix(cs.subast[0], prefix)
            set_node_prefix(node, cs.indent)

        cs.subast.append(node)

    def processing_Paren(self, node, parent, debug):
        res = self.processing(node.expr, parent, debug)

        if isinstance(res, c_ast.BinaryOp):
            # Node hasn't TCG var.
            # Return source ast.
            return node
        elif res is not None:
            return res
        else:
            return node.expr

    def processing_Compound(self, node, parent, debug):
        if node.block_items is None:
            return

        self.cs = CompoundState(prev_state = self.cs)
        cs = self.cs

        new_block_items = []
        for child in node.block_items:
            self.processing(child, node, debug)
            if cs.subast:
                new_block_items.extend(cs.subast)
                cs.subast = []
            else:
                new_block_items.append(child)

        node.block_items = new_block_items
        cs.insert_tcg_free(True)
        node.block_items.extend(cs.subast)
        cs.subast = []
        self.cs = cs.prev_state

    def processing_For(self, node, parent, debug):
        cs = self.cs
        indent = get_indent(node)
        if cs.indent is None:
            cs.indent = indent

        if debug:
            comment = cs.indent + DCP + 'for ('
            if node.init is not None:
                comment += self.generator.visit(node.init)
            comment += ';'
            if node.cond is not None:
                comment += self.generator.visit(node.cond)
            comment += ';'
            if node.next is not None:
                comment += self.generator.visit(node.next)
            comment += ')' + DCS
            prefix = insert_debug_info(node.prefix, comment)
        else:
            prefix = node.prefix

        self.processing(node.init, node, False)

        subast_init = cs.subast
        cs.subast = []

        brcond = self.handle_brcond(node)
        if brcond is None:
            return
        ast_free, ast_cond, brcond_id, expr_list = brcond
        ast_cond = ast_free + ast_cond

        label_cond = cs.tcg_label_decl()
        label_false = cs.tcg_label_decl()
        label_continue = cs.tcg_label_decl()

        self.break_label.append(label_false)
        self.continue_label.append([label_continue, False])

        cs.subast.append(c_ast.FuncCall(
            c_ast.ID('gen_set_label', prefix = indent),
            c_ast.ExprList([c_ast.ID(label_cond)])
        ))

        cs.subast = subast_init + cs.subast + ast_cond
        set_node_prefix(cs.subast[0], prefix)

        expr_list.append(c_ast.ID(label_false, prefix = ' '))
        cs.subast.append(c_ast.FuncCall(brcond_id, c_ast.ExprList(expr_list)))

        cs.insert_tcg_free()

        self.handle_br_statement(node, node.stmt, indent, debug)

        if self.continue_label[-1][1] is True:
            cs.subast.append(c_ast.FuncCall(
                c_ast.ID('gen_set_label', prefix = indent),
                c_ast.ExprList([c_ast.ID(label_continue)])
            ))
        else:
            for i, e in enumerate(cs.subast):
                if isinstance(e, c_ast.Decl) and e.name == label_continue:
                    cs.subast.pop(i)
                    break

        cs.insert_tcg_free()

        for_subast = cs.subast
        cs.subast = []
        self.processing(node.next, node, False)
        for e in self.cs.subast:
            set_node_prefix(e, indent)
        cs.subast = for_subast + cs.subast

        cs.insert_tcg_free()

        cs.subast.append(c_ast.FuncCall(
            c_ast.ID('tcg_gen_br', prefix = indent),
            c_ast.ExprList([c_ast.ID(label_cond)])
        ))

        set_label_func = gen_terminator_label(node.stmt, indent, label_false)
        if debug:
            set_label_func.name.prefix = cs.indent + \
                DCP + 'end of the "for"' + DCS +\
                set_label_func.name.prefix
        cs.subast.append(set_label_func)

        self.break_label.pop()
        self.continue_label.pop()

    def processing_DoWhile(self, node, parent, debug):
        cs = self.cs
        indent = get_indent(node)
        if cs.indent is None:
            cs.indent = indent

        brcond = self.handle_brcond(node)
        if brcond is None:
            return
        ast_free, ast_cond, brcond_id, expr_list = brcond

        cs.subast = ast_cond
        cs.subast.append(c_ast.FuncCall(brcond_id, c_ast.ExprList(expr_list)))
        cs.insert_tcg_free()

        cs.subast = ast_free

        label_true = cs.tcg_label_decl()
        label_false = cs.tcg_label_decl()
        label_continue = cs.tcg_label_decl()

        expr_list.append(c_ast.ID(label_false, prefix = ' '))

        self.break_label.append(label_false)
        self.continue_label.append([label_continue, False])

        cs.subast.append(c_ast.FuncCall(
            c_ast.ID('gen_set_label', prefix = indent),
            c_ast.ExprList([c_ast.ID(label_true)])
        ))

        self.handle_br_statement(node, node.stmt, indent, debug)

        if self.continue_label[-1][1] is True:
            cs.subast.append(c_ast.FuncCall(
                c_ast.ID('gen_set_label', prefix = indent),
                c_ast.ExprList([c_ast.ID(label_continue)])
            ))
        else:
            for i, e in enumerate(cs.subast):
                if isinstance(e, c_ast.Decl) and e.name == label_continue:
                    cs.subast.pop(i)
                    break

        cs.subast += ast_cond

        if debug:
            prefix = insert_debug_info(
                node.prefix,
                cs.indent + DCP + 'start of the "DoWhile"' + DCS
            )
        else:
            prefix = node.prefix
        set_node_prefix(cs.subast[0], prefix)

        cs.subast.append(c_ast.FuncCall(
            c_ast.ID('tcg_gen_br', prefix = indent),
            c_ast.ExprList([c_ast.ID(label_true)])
        ))

        set_label_func = gen_terminator_label(node.stmt, indent, label_false)

        if debug:
            comment = cs.indent + DCP +\
                'do {...} while (' + self.generator.visit(node.cond) + ')' +\
                DCS
            set_label_func.name.prefix = insert_debug_info(
                set_label_func.name.prefix,
                comment
            )
        cs.subast.append(set_label_func)

        cs.insert_tcg_free()

        self.break_label.pop()
        self.continue_label.pop()

    def processing_While(self, node, parent, debug):
        cs = self.cs
        indent = get_indent(node)
        if cs.indent is None:
            cs.indent = indent

        if debug:
            comment = cs.indent + DCP +\
                'while (' + self.generator.visit(node.cond) + ')' + DCS
            prefix = insert_debug_info(node.prefix, comment)
        else:
            prefix = node.prefix

        brcond = self.handle_brcond(node)
        if brcond is None:
            return

        ast_free, ast_cond, brcond_id, expr_list = brcond
        ast_cond = ast_free + ast_cond

        label_cond = cs.tcg_label_decl()
        label_false = cs.tcg_label_decl()

        self.break_label.append(label_false)
        self.continue_label.append([label_cond, False])

        cs.subast.append(c_ast.FuncCall(
            c_ast.ID('gen_set_label', prefix = indent),
            c_ast.ExprList([c_ast.ID(label_cond)])
        ))

        cs.subast += ast_cond
        set_node_prefix(cs.subast[0], prefix)

        expr_list.append(c_ast.ID(label_false, prefix = ' '))
        cs.subast.append(c_ast.FuncCall(brcond_id, c_ast.ExprList(expr_list)))

        cs.insert_tcg_free()
        self.handle_br_statement(node, node.stmt, indent, debug)
        cs.insert_tcg_free()

        cs.subast.append(c_ast.FuncCall(
            c_ast.ID('tcg_gen_br', prefix = indent),
            c_ast.ExprList([c_ast.ID(label_cond)])
        ))

        set_label_func = gen_terminator_label(node.stmt, indent, label_false)
        if debug:
            set_label_func.name.prefix = cs.indent + \
                DCP + 'end of the "while"' + DCS +\
                set_label_func.name.prefix
        cs.subast.append(set_label_func)

        self.break_label.pop()
        self.continue_label.pop()

    def processing_If(self, node, parent, debug, label_end = None):
        # If label_end is not None it't 'else if' processing
        cs = self.cs
        indent = get_indent(node)
        if cs.indent is None:
            cs.indent = indent
        if label_end is not None:
            indent = cs.indent

        if debug:
            debug_info = cs.indent + DCP +\
                'if (' + self.generator.visit(node.cond) + ')' + DCS

        cond = self.processing(node.cond, node, False)

        if isinstance(cond, tuple):
            # cond = (func name ID, func params)
            brcond = cond[0]
            expr_list = cond[1]
        else:
            cond_type, cond_suffix = get_node_desc(cond)
            if cond_type is None:
                # Cond isn't run time variable
                if cs.subast:
                    # this case can only happen when the condition
                    # contains a FuncCall with tcg param
                    cs.subast.append(node)
                self.processing(node.iftrue, node, debug)
                self.processing(node.iffalse, node, debug)
                return

            brcond = c_ast.ID('tcg_gen_brcondi' + cond_suffix)
            set_node_prefix(cond, ' ')
            expr_list = [
                c_ast.ID('TCG_COND_EQ'),
                cond,
                c_ast.Constant('int', '0', prefix = ' ')
            ]

        if node.iffalse is None and label_end is not None:
            label_iffalse = label_end
        else:
            label_iffalse = cs.tcg_label_decl()

        expr_list.append(c_ast.ID(label_iffalse, prefix = ' '))
        cs.subast.append(c_ast.FuncCall(brcond, c_ast.ExprList(expr_list)))

        # set indent to cond calculation
        if label_end is None:
            if debug:
                prefix = insert_debug_info(
                    node.prefix,
                    debug_info
                )
            else:
                prefix = node.prefix
        else:
            if debug:
                prefix = debug_info[:(len(indent) + len(DCP))] + 'else ' +\
                    debug_info[(len(indent) + len(DCP)):] + indent
            else:
                prefix = indent

        set_node_prefix(cs.subast[0], prefix)
        for e in cs.subast[1:]:
            set_node_prefix(e, indent)

        cs.insert_tcg_free()
        self.handle_br_statement(node, node.iftrue, indent, debug)
        cs.insert_tcg_free()

        if node.iffalse is None:
            cs.subast.append(
                gen_terminator_label(node.iftrue, indent, label_iffalse)
            )
            return

        # Else block
        if label_end is None:
            label_end = cs.tcg_label_decl()

        # save brace close prefix
        func_name = c_ast.ID('tcg_gen_br')
        if isinstance(node.iftrue, c_ast.Compound):
            func_name.prefix = node.iftrue.prefix[1]
            fixup_indent(func_name, indent)
        else:
            func_name.prefix = indent

        cs.subast.append(c_ast.FuncCall(
            func_name,
            c_ast.ExprList([c_ast.ID(label_end)])
        ))

        cs.subast.append(c_ast.FuncCall(
            c_ast.ID('gen_set_label', prefix = indent),
            c_ast.ExprList([c_ast.ID(label_iffalse)])
        ))
        set_node_prefix(
            cs.subast[-1],
            insert_debug_info(
                get_node_prefix(cs.subast[-1]),
                cs.indent + DCP + 'end of the "if"' + DCS
            )
        )

        if isinstance(node.iffalse, c_ast.If):
            if_subast = cs.subast
            cs.subast = []
            # 'processing_If' will call 'insert_tcg_free'
            self.processing_If(
                node.iffalse,
                node,
                debug,
                label_end = label_end
            )
            cs.subast = if_subast + cs.subast
        else:
            l = len(cs.subast)
            self.handle_br_statement(node, node.iffalse, indent, debug)
            cs.insert_tcg_free()
            cs.subast.append(
                gen_terminator_label(node.iffalse, indent, label_end)
            )

            if debug and len(cs.subast) > l:
                set_node_prefix(
                    cs.subast[l],
                    insert_debug_info(
                        get_node_prefix(cs.subast[l]),
                        cs.indent + DCP + 'else' + DCS
                    )
                )
                set_node_prefix(
                    cs.subast[-1],
                    insert_debug_info(
                        get_node_prefix(cs.subast[-1]),
                        cs.indent + DCP + 'end of the "else"' + DCS
                    )
                )

    def processing_Case(self, node, parent, debug):
        self.cs = CompoundState(prev_state = self.cs)
        cs = self.cs

        # We can get here only from Switch with non TCG cond
        self.break_label.append(None)
        stmts = []

        has_subast = False
        for child in node.stmts:
            # change parent to 'c_ast.Compound' if case has tcg expression
            # it's necessary for correct 'break' and 'contunue' work
            self.processing(child,
                c_ast.Compound([]) if has_subast else node,
                debug
            )
            if cs.subast:
                stmts += cs.subast
                cs.subast = []
                has_subast = True
            else:
                stmts.append(child)

        if has_subast:
            cs.insert_tcg_free(True)
            stmts += cs.subast
            for e in stmts:
                if isinstance(e, c_ast.Decl):
                    indent = get_indent(node)
                    stmts = [c_ast.Compound(stmts, prefix = [indent, indent])]
                    break

            cs.subast = []

        node.stmts = stmts
        self.break_label.pop()

        self.cs = cs.prev_state

    def processing_Default(self, node, parent, debug):
        self.processing_Case(node, parent, debug)

    def processing_Switch(self, node, parent, debug):
        cs = self.cs
        indent = get_indent(node)
        if cs.indent is None:
            cs.indent = indent

        if debug:
            comment = cs.indent + DCP +\
                'switch (' + self.generator.visit(node.cond) + ')' + DCS
            prefix = insert_debug_info(node.prefix, comment)
        else:
            prefix = node.prefix

        cond = self.processing(node.cond, node, debug)

        if isinstance(cond, tuple):
            raise ValueError(
"{}: comparisons tcg vars in switch conditional does't support".format(
                    node.coord
                )
            )

        cond_type, cond_suffix = get_node_desc(cond)

        if cond_type is None:
            self.processing(node.stmt, node, debug)
            return

        if cond.name in cs.tcg_tmp_list:
            local_var = cs.get_unoccupied_tmp_tcg(cond_type, True)
            cond.prefix = ' '
            self.cs.subast.append(c_ast.FuncCall(
                c_ast.ID('tcg_gen_mov' + cond_suffix),
                c_ast.ExprList([
                    local_var,
                    cond
                ]),
            ))
            cond = duplicate_simple_node(local_var)
            cond.prefix = ''

        for e in cs.subast:
            set_node_prefix(e, cs.indent)

        cs.insert_tcg_free()

        set_node_prefix(cond, ' ')
        label_end = cs.tcg_label_decl()
        set_node_prefix(cs.subast[0], prefix)
        self.break_label.append(label_end)

        label_list = []
        label_default = None
        for child in node.stmt:
            label = cs.tcg_label_decl()
            label_list.append(label)

            if isinstance(child, c_ast.Default):
                if label_default is not None:
                    raise RuntimeError(
                        '{}: "default" is duplicated'.format(child.coord)
                    )
                label_default = label
                continue

            if isinstance(child, c_ast.Case):
                child.expr.prefix = ' '
                cs.subast.append(c_ast.FuncCall(
                    c_ast.ID(
                        'tcg_gen_brcondi' + cond_suffix,
                        prefix = indent
                    ),
                    c_ast.ExprList([
                        c_ast.ID('TCG_COND_EQ'),
                        cond,
                        child.expr,
                        c_ast.ID(label, prefix = ' ')
                    ])
                ))

        cs.subast.append(
            c_ast.FuncCall(
                c_ast.ID(
                    'tcg_gen_brcondi' + cond_suffix,
                    prefix = indent
                ),
                c_ast.ExprList([
                    c_ast.ID('TCG_COND_ALWAYS'),
                    c_ast.ID('0', prefix = ' '),
                    c_ast.ID('0', prefix = ' '),
                    c_ast.ID(label_default, prefix = ' ')
                ])
            )
        )

        for i, child in enumerate(node.stmt):
            if debug:
                c = cs.indent + DCP
                if isinstance(child, c_ast.Case):
                    c += 'case' + self.generator.visit(child.expr) + ':'
                else:
                    c += 'default:'
                prefix = insert_debug_info(child.prefix, c + DCS)
            else:
                prefix = indent

            cs.subast.append(c_ast.FuncCall(
                c_ast.ID('gen_set_label', prefix = prefix),
                c_ast.ExprList([c_ast.ID(label_list[i])])
            ))

            if child.stmts:
                # body of case or default is not empty
                self.handle_br_statement(child, child.stmts, indent, debug)

            cs.insert_tcg_free()

        cs.subast.append(c_ast.FuncCall(
            c_ast.ID('gen_set_label', prefix = prefix),
            c_ast.ExprList([c_ast.ID(label_end)])
        ))

        if debug:
            set_node_prefix(
                cs.subast[-1],
                insert_debug_info(
                    get_node_prefix(cs.subast[-1]),
                    cs.indent + DCP + 'end of the "switch"' + DCS
                )
            )

        self.break_label.pop()

    def processing_Decl(self, node, parent, debug):
        cs = self.cs
        node_type = node.type

        if cs is not None:
            if cs.tcg_compound and node.name in cs.vars:
                count = 0
                new_name = node.name + '_' + str(count)
                while new_name in cs.vars:
                    count += 1
                    new_name = node.name + '_' + str(count)

                cs.n2c[node.name] = new_name
                print('Attention: {}: rename "{}" --> "{}"'.format(
                    node.coord, node.name, new_name)
                )
                node.name = new_name
                change_declname(node, new_name)

            # TODO: need a comment
            cs.vars.add(node.name)

        if isinstance(node_type, (c_ast.Enum, c_ast.Union, c_ast.Struct)):
            return
        elif isinstance(node_type, c_ast.FuncDecl):
            self.processing(node_type, node, debug)
            return

        # TODO: fixup ArrayDecl processing
        while isinstance(node_type, (c_ast.PtrDecl, c_ast.ArrayDecl)):
            node_type = node_type.type

        identifierType = node_type.type
        if not isinstance(identifierType, c_ast.IdentifierType):
            return

        if cs is not None:
            if cs.indent is None:
                if node.quals:
                    # decl has qualificators (const etc.)
                    cs.indent = get_indent(node)
                else:
                    cs.indent = get_indent(identifierType)

        type_names = identifierType.names
        if 'tcg' not in type_names:
            return

        count_identifiers = len(type_names)
        incorrect_names = False
        idx = None
        if count_identifiers > 3:
            incorrect_names = True
        elif count_identifiers == 3:
            if type_names[0] not in ('signed', 'unsigned'):
                incorrect_names = True
            idx = 1
        elif count_identifiers == 2:
            if type_names[0] not in ('signed', 'unsigned'):
                idx = 0

        var_type = 'TCGv'
        alloc_name = 'tcg_temp'

        if identifierType.is_local_tcg:
            alloc_name += '_local'
        alloc_name += '_new'

        if idx is not None:
            if type_names[idx] == 'short':
                alloc_name += '_i32'
                var_type += '_i32'
            elif type_names[idx] == 'long':
                alloc_name += '_i64'
                var_type += '_i64'
            else:
                incorrect_names = True

        if incorrect_names:
            raise TypeError(
                '{}: unsupported type names combination "{}"'.format(
                    node.coord,
                    ' '.join(type_names)
                )
            )

        identifierType.names = [var_type]

        # Param and extern hasn't cs
        if cs is not None:
            if isinstance(parent, c_ast.DeclList):
                if node is not parent.decls[0]:
                    identifierType.prefix = cs.indent
            cs.subast.append(node)

            nt = node.type
            indent = cs.indent
            if isinstance(nt, c_ast.ArrayDecl):
                if node.init:
                    raise RuntimeError(
                        str(node.coord) + ": tcg array init doesn't support"
                    )

                count = 0
                for_init = None
                for_free = None
                root_init = None
                var_init = None
                while isinstance(nt, c_ast.ArrayDecl):
                    counter = "i_" + str(count)
                    if isinstance(nt.dim, c_ast.Constant):
                        dim = c_ast.Constant(
                            nt.dim.type, nt.dim.value, prefix = ' '
                        )
                    else:
                        dim = c_ast.ID(
                            nt.dim.name, nt.dim.var_type, prefix = ' '
                        )

                    prev_for_init = for_init
                    prev_for_free = for_free

                    for_init = create_for_loop(counter, dim, indent)
                    for_free = create_for_loop(counter, dim, indent)

                    if count == 0:
                        root_init = for_init
                        root_free = for_free
                        var_init = c_ast.ArrayRef(
                            c_ast.ID(node.name), c_ast.ID(counter),
                        )
                        var_free = c_ast.ArrayRef(
                            c_ast.ID(node.name), c_ast.ID(counter),
                        )
                    else:
                        prev_for_init.stmt.block_items.append(for_init)
                        prev_for_free.stmt.block_items.append(for_free)
                        var_init = c_ast.ArrayRef(var_init, c_ast.ID(counter))
                        var_free = c_ast.ArrayRef(var_free, c_ast.ID(counter))

                    indent += cs.indent[1:]
                    count += 1
                    nt = nt.type

                var_init.prefix = indent
                for_init.stmt.block_items.append(
                    c_ast.Assignment(
                        '=',
                        var_init,
                        c_ast.FuncCall(
                            c_ast.ID(alloc_name, prefix = ' '),
                            None
                        )
                    )
                )

                for_free.stmt.block_items.append(
                    c_ast.FuncCall(
                        c_ast.ID("tcg_temp_free", prefix = indent),
                        c_ast.ExprList([var_free])
                    )
                )

                cs.subast.append(root_init)

                if identifierType.is_local_tcg:
                    cs.tcg_local_array_list.append(root_free)
                else:
                    cs.tcg_array_list.append(root_free)

                return

            if node.init is not None:
                self.processing(
                    c_ast.Assignment(
                        op = '=',
                        lvalue = c_ast.ID(
                            node.name,
                            var_type = type_names,
                            prefix = cs.indent
                        ),
                        rvalue = node.init
                    ),
                    node,
                    False
                )

        # Decl in FuncDef
        # We don't need to alloc
        if isinstance(parent, c_ast.ParamList):
            return

        # We don't need to alloc extern variable
        if 'extern' in node.storage:
            return

        if isinstance(parent, c_ast.FileAST) and 'extern' not in node.storage:
            print('ATTENTION: TCG variable "{}" is global'.format(node.name))

        node.init = c_ast.FuncCall(
            c_ast.ID(alloc_name, prefix = ' '),
            None
        )

        if identifierType.is_local_tcg:
            cs.tcg_local_list.append(node.name)
        else:
            cs.tcg_list.append(node.name)

    def processing_Cast(self, node, parent, debug):
        to_type = set(node.to_type.type.type.names)

        if 'tcg' not in to_type:
            return node

        res_var = None
        if isinstance(parent, c_ast.Assignment):
            plv_suffix = get_tcg_suffix(get_tcg_type(parent.lvalue))
            res_suffix = get_tcg_suffix(to_type)
            if plv_suffix == res_suffix:
                res_var = parent.lvalue
                set_node_prefix(res_var, '')

        return self.cs.cast(
            (self.processing(node.expr, node, debug), None, None),
            (res_var, to_type, None)
        )

    def processing_UnaryOp(self, node, parent, debug):
        op = node.op
        cs = self.cs

        if op in ('++', '--', 'p++', 'p--'):
            expr = self.processing(node.expr, node, False)
            expr_type, expr_suffix = get_node_desc(expr)
            if expr_type is None:
                return node

            if not isinstance(expr, (c_ast.ArrayRef, c_ast.ID)):
                raise ValueError(
                    '{}: unsupported UnaryOp "{}" with {}'.format(
                        node.coord,
                        op,
                        expr.__class__.__name__
                    )
                )

            if op.startswith('p'):
                # postfix '++' or '--'
                expr_id = get_id(node.expr)
                inc_dec_prefix = get_indent(expr_id)
                op = op[1:]
                param = duplicate_simple_node(expr)
                set_node_prefix(param, ' ')
                ret = cs.get_unoccupied_tmp_tcg(expr_type)
                if debug:
                    p = expr_id.prefix
                    expr_id.prefix = ''
                    comment = cs.indent + DCP +\
                        self.generator.visit(node) + DCS
                    prefix = insert_debug_info(p, comment)
                    expr_id.prefix = p
                else:
                    prefix = expr_id.prefix

                cs.subast.append(c_ast.FuncCall(
                    c_ast.ID(
                        'tcg_gen_mov' + expr_suffix,
                        prefix = prefix
                    ),
                    c_ast.ExprList([
                        duplicate_simple_node(ret),
                        param
                    ]),
                ))
            else:
                ret = duplicate_simple_node(expr)
                if debug:
                    p = node.prefix
                    node.prefix = ''
                    comment = cs.indent + DCP +\
                        self.generator.visit(node) + DCS
                    inc_dec_prefix = insert_debug_info(p, comment)
                    node.prefix = p
                else:
                    inc_dec_prefix = node.prefix

            if op == '++':
                func_name = 'tcg_gen_addi'
            else:
                func_name = 'tcg_gen_subi'

            param = duplicate_simple_node(expr)
            set_node_prefix(expr, ' ')
            set_node_prefix(param, '')
            cs.subast.append(c_ast.FuncCall(
                c_ast.ID(func_name + expr_suffix, prefix = inc_dec_prefix),
                c_ast.ExprList([
                    param,
                    expr,
                    c_ast.Constant('int', '1', prefix = ' ')
                ]),
            ))
        else:
            # other UnaryOp without side effects
            expr = self.processing(node.expr, node, False)
            expr_type = get_tcg_type(expr)
            if expr_type is None:
                return node

            set_node_prefix(expr, ' ')
            suffix = get_tcg_suffix(expr_type)
            if op == '+':
                return expr
            elif op == '-':
                func_name = 'tcg_gen_neg'
            elif op == '~':
                func_name = 'tcg_gen_not'
            elif op == '!':
                # expr EQ 0
                # 0 --> 1
                # 1 --> 0
                if isinstance(
                    parent, (c_ast.If, c_ast.While, c_ast.DoWhile, c_ast.For)
                ):
                    # avoid extra setcond
                    return (
                        c_ast.ID('tcg_gen_brcondi' + suffix),
                        [
                            # r_cond = expr NE 0
                            c_ast.ID('TCG_COND_NE'),
                            expr,
                            c_ast.Constant('int', '0', prefix = ' ')
                        ]
                    )
                else:
                    func_name = 'tcg_gen_setcondi'
            else:
                raise ValueError(
                    '{}: unsupported UnaryOp "{}" with tcg operand'.format(
                        node.coord, op
                    )
                )

            func_name += suffix
            res_var = None
            res_var_exist = False
            if isinstance(parent, c_ast.Assignment):
                plv_suffix = get_tcg_suffix(get_tcg_type(parent.lvalue))
                if plv_suffix == suffix:
                    res_var_exist = True
                    res_var = parent.lvalue

            if res_var is None:
                # Try to reuse tmp variable that are already used in binary Op
                if isinstance(expr, c_ast.ID):
                    p1_name = expr.name
                    for v in cs.tcg_tmp_list:
                        if v == p1_name:
                            res_var = c_ast.ID(v)
                            break

                if res_var is None:
                    # look up for tmp that has already been created
                    res_var = cs.get_unoccupied_tmp_tcg(expr_type)

            if op == '!':
                param_list = [
                    c_ast.ID('TCG_COND_EQ'),
                    res_var,
                    expr,
                    c_ast.Constant('int', '0', prefix = ' ')
                ]
                set_node_prefix(res_var, ' ')
            else:
                param_list = [res_var, expr]
                set_node_prefix(res_var, '')

            cs.subast.append(c_ast.FuncCall(
                c_ast.ID(func_name),
                c_ast.ExprList(param_list),
            ))

            if res_var_exist:
                # avoid extra assign or setcond
                ret = res_var
            else:
                ret = c_ast.ID(res_var.name, var_type = expr_type)

        return ret

    def processing_arithmetic_bin_op(self, node, parent, debug):
        lv = self.processing(node.left, node, debug)
        if lv is None:
            lv = node.left

        rv = self.processing(node.right, node, debug)
        if rv is None:
            rv = node.right

        func_name, support_int_rv, is_commutative = binary_op[node.op]

        param_desc = self.cs.get_bin_expr_param(rv, lv, is_commutative,
            support_int_rv,
            r_shift = isinstance(func_name, tuple)
        )
        if param_desc is None:
            return node
        res_type, suffix, i_str, p1, p2 = param_desc

        cs = self.cs

        if isinstance(func_name, tuple):
            # op is logical or arithmetics right shift
            if 'signed' in res_type:
                # shr
                func_name = func_name[1]
            else:
                # sar
                func_name = func_name[0]

        if not (support_int_rv or 'signed' in res_type):
            # only for '/' and '%'
            func_name += 'u'
        else:
            func_name += i_str

        func_name += suffix
        res_var = None
        res_var_exist = False
        if isinstance(parent, c_ast.Assignment):
            plv_suffix = get_tcg_suffix(get_tcg_type(parent.lvalue))
            if plv_suffix == suffix:
                res_var_exist = True
                res_var = parent.lvalue
                set_node_prefix(res_var, '')

        if res_var is None:
            # Try to reuse tmp variable that are already used in binary Op
            if isinstance(p1, c_ast.ID):
                p1_name = p1.name
                for v in cs.tcg_tmp_list:
                    if v == p1_name:
                        res_var = c_ast.ID(v)
                        break

            if isinstance(p2, c_ast.ID):
                p2_name = p2.name
                for i, v in enumerate(cs.tcg_tmp_list):
                    if v == p2_name:
                        if res_var is None:
                            res_var = c_ast.ID(v)
                        else:
                            cs.tcg_tmp_list_hold[i] = False
                        break

            if res_var is None:
                # look up for tmp that has already been created
                res_var = cs.get_unoccupied_tmp_tcg(res_type)

        if isinstance(p1, c_ast.Paren):
            p1 = p1.expr
        if isinstance(p2, c_ast.Paren):
            p2 = p2.expr

        set_node_prefix(p1, ' ')
        set_node_prefix(p2, ' ')

        self.cs.subast.append(c_ast.FuncCall(
            c_ast.ID(func_name),
            c_ast.ExprList([res_var, p1, p2])
        ))

        if res_var_exist:
            # avoid extra assign
            # parent keep res var
            return res_var
        else:
            return c_ast.ID(res_var.name, var_type = res_type)

    def processing_logical_bin_op(self, node, parent, debug):
        lv = self.processing(node.left, node, debug)
        if lv is None:
            lv = node.left

        rv = self.processing(node.right, node, debug)
        if rv is None:
            rv = node.right

        op = node.op
        cs = self.cs

        is_commutative = op not in ('>=', '<=', '>', '<')
        param_desc = self.cs.get_bin_expr_param(rv, lv, is_commutative, True)
        if param_desc is None:
            return None
        res_type, suffix, i_str, p1, p2 = param_desc

        res_var = None
        res_var_exist = False
        if not (
            isinstance(
                parent,
                (c_ast.If, c_ast.While, c_ast.DoWhile, c_ast.For)
            )
            and op in ('||', '==', '!=', '>', '>=', '<', '<=')
        ):
            if isinstance(parent, c_ast.Assignment):
                plv_suffix = get_tcg_suffix(get_tcg_type(parent.lvalue))
                if plv_suffix == suffix:
                    res_var_exist = True
                    res_var = parent.lvalue

            if res_var is None:
                res_var = None
                # Try to reuse tmp variable that are already used in binary Op
                if isinstance(p1, c_ast.ID):
                    p1_name = p1.name
                    for v in cs.tcg_tmp_list:
                        if v == p1_name:
                            res_var = c_ast.ID(v, var_type = res_type)
                            break

                if isinstance(p2, c_ast.ID):
                    p2_name = p2.name
                    for i, v in enumerate(cs.tcg_tmp_list):
                        if v == p2_name:
                            if res_var is None:
                                res_var = c_ast.ID(v, var_type = res_type)
                            else:
                                cs.tcg_tmp_list_hold[i] = False
                            break

                if res_var is None:
                    # look up for tmp that has already been created
                    res_var = cs.get_unoccupied_tmp_tcg(res_type)

            set_node_prefix(res_var, ' ')

        if isinstance(p1, c_ast.Paren):
            p1 = p1.expr
        if isinstance(p2, c_ast.Paren):
            p2 = p2.expr

        set_node_prefix(p1, ' ')
        set_node_prefix(p2, ' ')

        if op == '||':
            # (p1 | p2) NE 0
            if res_var is None:
                or_res = cs.get_unoccupied_tmp_tcg(res_type)
            else:
                or_res = duplicate_simple_node(res_var)
                set_node_prefix(or_res, '')

            self.cs.subast.append(c_ast.FuncCall(
                c_ast.ID('tcg_gen_or' + i_str + suffix),
                c_ast.ExprList([or_res, p1, p2])
            ))

            or_res = duplicate_simple_node(or_res)
            set_node_prefix(or_res, ' ')

            if res_var is None:
                # avoid extra setcond
                res_var = (
                    c_ast.ID('tcg_gen_brcond' + i_str + suffix),
                    [
                        c_ast.ID('TCG_COND_EQ'),
                        or_res,
                        c_ast.Constant('int', '0', prefix = ' ')
                    ]
                )
            else:
                self.cs.subast.append(c_ast.FuncCall(
                    c_ast.ID('tcg_gen_setcond' + i_str + suffix),
                    c_ast.ExprList([
                        c_ast.ID('TCG_COND_NE'),
                        res_var,
                        or_res,
                        c_ast.Constant('int', '0', prefix = ' ')
                    ])
                ))
        elif op == '&&':
            # (p1 NE 0) & (p2 NE 0)
            cond_res1 = duplicate_simple_node(res_var)
            self.cs.subast.append(c_ast.FuncCall(
                c_ast.ID('tcg_gen_setcondi' + suffix),
                c_ast.ExprList([
                    c_ast.ID('TCG_COND_NE'),
                    cond_res1,
                    p1,
                    c_ast.Constant('int', '0', prefix = ' ')
                ])
            ))
            p1 = duplicate_simple_node(cond_res1)

            if i_str == 'i':
                p2 = c_ast.BinaryOp(
                    op = '!=',
                    left = p2,
                    right = c_ast.Constant('int', '0', prefix = ' '),
                    prefix = ' '
                )
            else:
                cond_res2 = cs.get_unoccupied_tmp_tcg(res_type)
                cond_res2.prefix = ' '
                cs.subast.append(c_ast.FuncCall(
                    c_ast.ID('tcg_gen_setcondi' + suffix),
                    c_ast.ExprList([
                        c_ast.ID('TCG_COND_NE'),
                        cond_res2,
                        p2,
                        c_ast.Constant('int', '0', prefix = ' ')
                    ])
                ))
                p2 = duplicate_simple_node(cond_res2)

            set_node_prefix(res_var, '')
            cs.subast.append(c_ast.FuncCall(
                c_ast.ID('tcg_gen_and' + i_str + suffix),
                c_ast.ExprList([
                    res_var,
                    p1,
                    p2
                ])
            ))
        else:
            if op == '==':
                cond = 'TCG_COND_EQ'
                r_cond = 'TCG_COND_NE'
            elif op == '!=':
                cond = 'TCG_COND_NE'
                r_cond = 'TCG_COND_EQ'
            else:
                if op == '>=':
                    cond = 'TCG_COND_GE'
                    r_cond = 'TCG_COND_LT'
                elif op == '<=':
                    cond = 'TCG_COND_LE'
                    r_cond = 'TCG_COND_GT'
                elif op == '>':
                    cond = 'TCG_COND_GT'
                    r_cond = 'TCG_COND_LE'
                elif op == '<':
                    cond = 'TCG_COND_LT'
                    r_cond = 'TCG_COND_GE'
                else:
                    raise ValueError(
                        '{}: unsupported BinaryOp "{}" with tcg operand'.format(
                            node.coord,
                            op
                        )
                    )

                if 'signed' not in res_type:
                    cond += 'U'
                    r_cond += 'U'

            if res_var is None:
                # avoid extra setcond
                res_var = (
                    c_ast.ID('tcg_gen_brcond' + i_str + suffix),
                    [c_ast.ID(r_cond), p1, p2]
                )
            else:
                self.cs.subast.append(c_ast.FuncCall(
                    c_ast.ID('tcg_gen_setcond' + i_str + suffix),
                    c_ast.ExprList([c_ast.ID(cond), res_var, p1, p2])
                ))

        if res_var_exist or isinstance(res_var, tuple):
            # avoid extra assign or setcond
            # parent keep res var
            return res_var
        else:
            return c_ast.ID(res_var.name, var_type = res_type)

    def processing_BinaryOp(self, node, parent, debug):
        if node.op in binary_op:
            return self.processing_arithmetic_bin_op(node, parent, debug)
        else:
            return self.processing_logical_bin_op(node, parent, debug)

    def processing_Assignment(self, node, parent, debug):
        cs = self.cs
        nlv = node.lvalue
        lvalue = self.processing(nlv, node, False)
        l_type, l_suffix = get_node_desc(lvalue)
        l_id = get_id(lvalue)
        if cs.indent is None:
            cs.indent = get_indent(l_id)

        if debug:
            p = get_node_prefix(nlv)
            set_node_prefix(nlv, '')
            c = cs.indent + DCP + self.generator.visit(node) + DCS
            set_node_prefix(nlv, p)
            lv_prefix = insert_debug_info(l_id.prefix, c)
        else:
            lv_prefix = l_id.prefix

        if node.op != '=':
            if l_type is not None:
                bo = c_ast.BinaryOp(
                    node.op[:-1],
                    duplicate_simple_node(lvalue),
                    node.rvalue
                )
                node.op = '='
                node.rvalue = bo

        rvalue = self.processing(node.rvalue, node, False)
        if rvalue is None:
            rvalue = node.rvalue

        r_type, r_suffix = get_node_desc(rvalue)

        if l_type is None:
            if r_type is not None:
                raise TypeError(
                    '{}: assignment TCG expression to non TCG var'.format(
                        node.coord
                    )
                )
            if cs.subast:
                # this case can only happen when the assignment
                # contains a FuncCall with tcg param
                cs.subast.append(node)
            return

        func_name = 'tcg_gen_mov'
        if r_type is None:
            func_name += 'i' + l_suffix
        else:
            if l_suffix == r_suffix:
                func_name += l_suffix
            else:
                rvalue = cs.cast(
                    (rvalue, r_type, r_suffix),
                    (lvalue, l_type, l_suffix)
                )

        # result isn't in lvalue
        if rvalue is not lvalue:
            set_node_prefix(lvalue, '')
            set_node_prefix(rvalue, ' ')
            cs.subast.append(c_ast.FuncCall(
                c_ast.ID(func_name),
                c_ast.ExprList([lvalue, rvalue])
            ))

        # Unlock last hold tcg tmp
        tcg_tmp_list_hold = cs.tcg_tmp_list_hold
        for i, e in enumerate(tcg_tmp_list_hold):
            if e:
                tcg_tmp_list_hold[i] = False
                break

        if not isinstance(parent, c_ast.Decl):
            set_node_prefix(cs.subast[0], lv_prefix)
        # else it's assignment in declaration
        # and prefix was set in processing_Decl
        for e in cs.subast[1:]:
            set_node_prefix(e, cs.indent)

        return duplicate_simple_node(lvalue)

    def handle_brcond(self, node):
        cs = self.cs
        # If cond have tcg type we insert label under cond processing
        # Otherwise we restore cs.tcg_list and cs.tcg_tmp_list
        tcg_list = cs.tcg_list
        tcg_tmp_list = cs.tcg_tmp_list
        tcg_tmp_list_hold = cs.tcg_tmp_list_hold
        cs.insert_tcg_free()

        ast_free = cs.subast
        cs.subast = []

        cond = self.processing(node.cond, node)
        ast_cond = cs.subast
        for e in ast_cond:
            set_node_prefix(e, cs.indent)
        cs.subast = []

        if isinstance(cond, tuple):
            # cond = (func name ID, func params)
            brcond_id = cond[0]
            brcond_id.prefix = cs.indent
            return (ast_free, ast_cond, brcond_id, cond[1])
        else:
            cond_type, cond_suffix = get_node_desc(cond)
            if cond_type is None:
                # Cond isn't run time variable
                self.break_label.append(None)
                self.continue_label.append(None)
                cs.subast = []

                cs.tcg_list = tcg_list
                cs.tcg_tmp_list = tcg_tmp_list
                cs.tcg_tmp_list_hold = tcg_tmp_list_hold

                # XXX: because of `cs.subast = []` above, this is `False`
                if cs.subast:
                    # this case can only happen when the condition
                    # contains a FuncCall with tcg param
                    cs.subast.append(node)
                self.processing(node.stmt, node)

                self.break_label.pop()
                self.continue_label.pop()
                return None

            brcond_id = c_ast.ID(
                'tcg_gen_brcondi' + cond_suffix,
                prefix = cs.indent
            )
            set_node_prefix(cond, ' ')
            expr_list = [
                c_ast.ID('TCG_COND_EQ'),
                cond,
                c_ast.Constant('int', '0', prefix = ' ')
            ]

            return (ast_free, ast_cond, brcond_id, expr_list)

    # Used to handle stmt in If, For, While, DoWhile, Case, Default
    def handle_br_statement(self, node, processing_child, indent, debug):
        cs = self.cs
        local_subast = cs.subast
        cs.subast = []

        if isinstance(node, (c_ast.Case, c_ast.Default)):
            if isinstance(processing_child[0], c_ast.Compound):
                processing_list = processing_child[0].block_items

                prev_tcg_compound = cs.tcg_compound
                cs.tcg_compound = True
                prev_n2c = cs.n2c
                cs.n2c = cs.n2c.copy()
            else:
                processing_list = processing_child
        else:
            if isinstance(processing_child, c_ast.Compound):
                processing_list = processing_child.block_items

                prev_tcg_compound = cs.tcg_compound
                cs.tcg_compound = True
                prev_n2c = cs.n2c
                cs.n2c = cs.n2c.copy()
            else:
                processing_list = [processing_child]

        if processing_list is not None:
            for child in processing_list:
                self.processing(child, node, debug)
                if cs.subast:
                    for e in cs.subast:
                        fixup_indent(e, indent)
                    local_subast.extend(cs.subast)
                    cs.subast = []
                else:
                    fixup_indent(child, indent)
                    local_subast.append(child)

        if isinstance(processing_child, c_ast.Compound):
            cs.tcg_compound = prev_tcg_compound
            cs.n2c = prev_n2c

        cs.subast = local_subast


def create_for_loop(count_name, const, indent):
    return c_ast.For(
        c_ast.Decl(
            name = count_name,
            quals = [], storage = [], funcspec = [],
            type = c_ast.TypeDecl(
                declname = count_name,
                type = c_ast.IdentifierType(['int']),
                quals = [],
                prefix = ' '
            ),
            init = c_ast.Constant("int", "0", prefix = ' '),
            bitsize = None
        ),
        c_ast.BinaryOp(
            '<',
            c_ast.ID(count_name, prefix = " "),
            const,
            prefix = ' '
        ),
        c_ast.UnaryOp(
            "++",
            c_ast.ID(count_name),
            prefix = " "
        ),
        c_ast.Compound([], prefix = [' ', indent]),
        prefix = indent
    )


def determine_var_type(node, ns = {}, func_params = {}):
    """ Determine variable (ID) type.

        ns is dictonary containing current name space:
            Key: ID name
            Value: [ID type, is_local]
    """

    if isinstance(
        node,
        (
            c_ast.Label,
            c_ast.Goto,
            c_ast.DoWhile,
            c_ast.While,
            c_ast.Case,
            c_ast.Default,
        )
    ):
        for v in ns.values():
            v[1] = True
        for __, child in node.children():
            determine_var_type(child, ns, func_params)

    elif isinstance(node, c_ast.FuncCall):
        for v in ns.values():
            v[1] = True
        if node.name.name in func_params:
            node.args_type = func_params[node.name.name]

        if node.args:
            determine_var_type(node.args, ns, func_params)

    elif isinstance(node, c_ast.For):
        if node.init is not None:
            determine_var_type(node.init, ns, func_params)
        for v in ns.values():
            v[1] = True
        if node.cond is not None:
            determine_var_type(node.cond, ns, func_params)
        if node.next is not None:
            determine_var_type(node.next, ns, func_params)
        determine_var_type(node.stmt, ns, func_params)

    elif isinstance(node, c_ast.If):
        determine_var_type(node.cond, ns, func_params)
        for v in ns.values():
            v[1] = True
        determine_var_type(node.iftrue, ns, func_params)
        if node.iffalse is not None:
            determine_var_type(node.iffalse, ns, func_params)

    elif isinstance(node, c_ast.Compound):
        if node.block_items is not None:
            # We don't need to deepcopy namespace.
            # If new variable has been declared,
            # list link will be changed to new.
            # If variable has been used,
            # value in all parent name space will be updated.
            ns = ns.copy()
            for child in node.block_items:
                determine_var_type(child, ns, func_params)

    elif isinstance(node, c_ast.FuncDef):
        func_name = node.decl.type.type.declname
        func_params[func_name] = []
        body_ns = ns.copy()
        for p in node.decl.type.args.params:
            while not (
                    isinstance(p, c_ast.TypeDecl)
                and isinstance(p.type, c_ast.IdentifierType)
            ):
                p = p.type
            body_ns[p.declname] = [p.type, False]
            func_params[func_name].append(p.type.names)

        determine_var_type(node.body, body_ns)

    elif isinstance(node, c_ast.TypeDecl) \
     and isinstance(node.type, c_ast.IdentifierType):
        # TODO: support typedef
        ns[node.declname] = [node.type, False]

    elif isinstance(node, c_ast.ID):
        if node.name in ns:
            id_desc = ns[node.name]
            node.var_type = id_desc[0].names
            if id_desc[1]:
                l = node.var_type
                if 'tcg' in l:
                    id_desc[0].is_local_tcg = True

    else:
        for __, child in node.children():
            determine_var_type(child, ns, func_params)


def convert_i3s_to_c(ast, debug = False):
    determine_var_type(ast)
    i3s_class = I3SProcessing()
    i3s_class.processing(ast, None, debug)
