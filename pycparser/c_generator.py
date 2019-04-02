#------------------------------------------------------------------------------
# pycparser: c_generator.py
#
# C code generator from pycparser AST nodes.
#
# Eli Bendersky [https://eli.thegreenplace.net/]
# License: BSD
#------------------------------------------------------------------------------
from . import c_ast


class CGenerator(object):
    """ Uses the same visitor pattern as c_ast.NodeVisitor, but modified to
        return a value from each visit method, using string accumulation in
        generic_visit.
    """

    def visit(self, node):
        method = 'visit_' + node.__class__.__name__
        res = getattr(self, method, self.generic_visit)(node)
        if not isinstance(
            node,
            (c_ast.Compound,
             c_ast.BinaryOp,
             c_ast.Assignment,
             c_ast.Enum,
             c_ast.Struct,
             c_ast.TernaryOp,
             c_ast.Union
             )
        ):
            res = node.prefix + res
        return res

    def generic_visit(self, node):
        #~ print('generic:', type(node))
        if node is None:
            return ''
        else:
            return ''.join(self.visit(c) for c_name, c in node.children())

    def visit_Constant(self, n):
        return n.value

    def visit_ID(self, n):
        return n.name

    def visit_Pragma(self, n):
        ret = '#pragma'
        if n.string:
            ret += ' ' + n.string
        return ret

    def visit_ArrayRef(self, n):
        return self._visit_expr(n.name) + '[' + self.visit(n.subscript) + ']'

    def visit_StructRef(self, n):
        return self._visit_expr(n.name) + n.type + self.visit(n.field)

    def visit_FuncCall(self, n):
        s = self.visit(n.args) if n.args else ''
        return self._visit_expr(n.name) + '(' + s +  ')'

    def visit_UnaryOp(self, n):
        operand = self._visit_expr(n.expr)
        if n.op == 'p++':
            return '%s++' % operand
        elif n.op == 'p--':
            return '%s--' % operand
        elif n.op == 'sizeof':
            s = self.visit(n.expr)
            if isinstance(n.expr.expr, c_ast.Typename):
                # delete extra whitespace when we take size of type
                s = s[:-1] + ')'
            return 'sizeof' + s
        else:
            return '%s%s' % (n.op, operand)

    def visit_BinaryOp(self, n):
        lval_str = self._visit_expr(n.left)
        rval_str = self._visit_expr(n.right)
        return '%s%s%s%s' % (lval_str, n.prefix, n.op, rval_str)

    def visit_Assignment(self, n):
        rval_str = self._visit_expr(n.rvalue)
        return '%s%s%s%s' % (self.visit(n.lvalue), n.prefix, n.op, rval_str)

    def visit_IdentifierType(self, n):
        return ' '.join(n.names)

    def _visit_expr(self, n):
        if isinstance(n, c_ast.InitList):
            return ' {' + self.visit(n) + '}'
        elif isinstance(n, c_ast.ExprList):
            return '(' + self.visit(n) + ')'
        else:
            return self.visit(n)

    def visit_Decl(self, n, no_type=False):
        # no_type is used when a Decl is part of a DeclList, where the type is
        # explicitly only for the first declaration in a list.
        #
        if no_type:
            s = self._generate_type(n.type, no_type=no_type)
        else:
            s = self._generate_decl(n)
        if n.bitsize: s += ' :' + self.visit(n.bitsize)
        if n.init:
            s += ' =' + self._visit_expr(n.init)
        return s

    def visit_DeclList(self, n):
        s = self.visit(n.decls[0])
        if len(n.decls) > 1:
            s += ',' + ','.join(
                self.visit_Decl(decl, no_type=True) for decl in n.decls[1:]
            )
        return s

    def visit_DeclListState(self, n):
        return self.visit(n.decl_list)

    def visit_Typedef(self, n):
        s = ''
        if n.storage: s += ' '.join(n.storage)
        s += self._generate_type(n.type)
        return s

    def visit_Cast(self, n):
        s = '(' + self._generate_type(n.to_type) + ')'
        return s + self.visit(n.expr)

    def visit_ExprList(self, n):
        visited_subexprs = []
        for expr in n.exprs:
            visited_subexprs.append(self._visit_expr(expr))
        return ','.join(visited_subexprs)

    def visit_InitList(self, n):
        visited_subexprs = []
        for expr in n.exprs:
            visited_subexprs.append(self._visit_expr(expr))
        return ','.join(visited_subexprs)

    def visit_Enum(self, n):
        return self._generate_struct_union_enum(n, name='enum')

    def visit_Enumerator(self, n):
        if not n.value:
            return n.name + ','
        else:
            return n.name +  ' =' + self.visit(n.value) + ','

    def visit_FuncDef(self, n):
        decl = self.visit(n.decl)
        body = self.visit(n.body)
        if n.param_decls:
            knrdecls = ';'.join(self.visit(p) for p in n.param_decls)
            return decl + knrdecls + ';' + body
        else:
            return decl + body

    def visit_FileAST(self, n):
        s = ''
        for ext in n.ext:
            if isinstance(ext, c_ast.FuncDef):
                s += self.visit(ext)
            elif isinstance(ext, c_ast.Pragma):
                s += self.visit(ext)
            else:
                s += self.visit(ext) + ';'
        return s

    def visit_Compound(self, n):
        s = n.prefix[0] + '{'
        if n.block_items:
            s += ''.join(self._generate_stmt(stmt) for stmt in n.block_items)
        s += n.prefix[1] + '}'
        return s

    def visit_CompoundLiteral(self, n):
        return '(' + self.visit(n.type) + '){' + self.visit(n.init) + '}'


    def visit_EmptyStatement(self, n):
        return ';'

    def visit_ParamList(self, n):
        return ','.join(self.visit(param) for param in n.params)

    def visit_Paren(self, n):
        return '(' + self.visit(n.expr) + ')'

    def visit_Return(self, n):
        s = 'return'
        if n.expr: s += self.visit(n.expr)
        return s + ';'

    def visit_Break(self, n):
        return 'break;'

    def visit_Continue(self, n):
        return 'continue;'

    def visit_TernaryOp(self, n):
        s = self._visit_expr(n.cond) + ' ?' + \
            self._visit_expr(n.iftrue) + ' :' + self._visit_expr(n.iffalse)
        return s

    def visit_If(self, n):
        s = 'if ('
        if n.cond: s += self.visit(n.cond)
        s += ')'
        s += self._generate_stmt(n.iftrue)
        if n.iffalse:
            s += ' else'
            s += self._generate_stmt(n.iffalse)
        return s

    def visit_For(self, n):
        s = 'for ('
        if n.init: s += self.visit(n.init)
        s += ';'
        if n.cond: s += self.visit(n.cond)
        s += ';'
        if n.next: s +=  self.visit(n.next)
        s += ')'
        s += self._generate_stmt(n.stmt)
        return s

    def visit_While(self, n):
        s = 'while ('
        if n.cond: s += self.visit(n.cond)
        s += ')'
        s += self._generate_stmt(n.stmt)
        return s

    def visit_DoWhile(self, n):
        s = 'do'
        s += self._generate_stmt(n.stmt)
        s += ' while ('
        if n.cond: s += self.visit(n.cond)
        s += ');'
        return s

    def visit_Switch(self, n):
        s = 'switch (' + self.visit(n.cond) + ')'
        s += self._generate_stmt(n.stmt)
        return s

    def visit_Case(self, n):
        s = 'case' + self.visit(n.expr) + ':'
        for stmt in n.stmts:
            s += self._generate_stmt(stmt)
        return s

    def visit_Default(self, n):
        s = 'default:'
        for stmt in n.stmts:
            s += self._generate_stmt(stmt)
        return s

    def visit_Label(self, n):
        return n.name + ':' + self._generate_stmt(n.stmt)

    def visit_Goto(self, n):
        return 'goto ' + n.name + ';'

    def visit_EllipsisParam(self, n):
        return '...'

    def visit_Struct(self, n):
        return self._generate_struct_union_enum(n, 'struct')

    def visit_Typename(self, n):
        return self._generate_type(n.type)

    def visit_Union(self, n):
        return self._generate_struct_union_enum(n, 'union')

    def visit_NamedInitializer(self, n):
        s = ''
        for name in n.name:
            if isinstance(name, c_ast.ID):
                s += '.' + name.name
            else:
                s += '[' + self.visit(name) + ']'
        s += ' = ' + self._visit_expr(n.expr)
        return s

    def visit_FuncDecl(self, n):
        return self._generate_type(n)

    def visit_ArrayDecl(self, n):
        return self._generate_type(n, emit_declname=False)
        
    def visit_TypeDecl(self, n):
        return self._generate_type(n, emit_declname=False)
        
    def visit_PtrDecl(self, n):
        return self._generate_type(n, emit_declname=False)

    def _generate_struct_union_enum(self, n, name):
        """ Generates code for structs, unions, and enums. name should be
            'struct', 'union', or 'enum'.
        """
        if name in ('struct', 'union'):
            members = n.decls
            body_function = self._generate_struct_union_body
        else:
            assert name == 'enum'
            members = None if n.values is None else n.values.enumerators
            body_function = self._generate_enum_body
        if isinstance(n.prefix, list):
            s = n.prefix[0] + name
        else:
            # n is IdentifierType
            s = n.prefix + name
        if n.name:
            s += ' ' + n.name
        if members is not None:
            # None means no members
            # Empty sequence means an empty list of members
            s += n.prefix[1] + '{'
            s += body_function(members)
            s += n.prefix[2] + '}'
        return s

    def _generate_struct_union_body(self, members):
        return ''.join(self._generate_stmt(decl) for decl in members)

    def _generate_enum_body(self, members):
        # `[:-2] + '\n'` removes the final `,` from the enumerator list
        return ''.join(self.visit(value) for value in members)[:-1]

    def _generate_stmt(self, n):
        """ Generation from a statement node. This method exists as a wrapper
            for individual visit_* methods to handle different treatment of
            some statements in this context.
        """
        typ = type(n)

        if typ in (
                c_ast.Decl, c_ast.DeclListState, c_ast.Assignment, c_ast.Cast,
                c_ast.UnaryOp, c_ast.BinaryOp, c_ast.TernaryOp, c_ast.FuncCall,
                c_ast.ArrayRef, c_ast.StructRef, c_ast.Constant, c_ast.ID,
                c_ast.Typedef, c_ast.ExprList
        ):
            # These can also appear in an expression context so no semicolon
            # is added to them automatically
            #
            return self.visit(n) + ';'
        else:
            return self.visit(n)

    def _generate_decl(self, n):
        """ Generation from a Decl node.
        """
        s = ''
        if n.storage:
            s = ' '.join(n.storage)
        if n.funcspec:
            s += ' ' + ' '.join(n.funcspec)
        s += self._generate_type(n.type)
        return s

    def _generate_type(self, n, modifiers=[], no_type = False, emit_declname = True):
        """ Recursive generation from a type node. n is the type node.
            modifiers collects the PtrDecl, ArrayDecl and FuncDecl modifiers
            encountered on the way down to a TypeDecl, to allow proper
            generation from it.
        """
        typ = type(n)
        #~ print(n, modifiers)

        if typ == c_ast.TypeDecl:
            s = ''
            if not no_type:
                if n.quals: s += ' '.join(n.quals)
                s += self.visit(n.type)

            nstr = n.prefix + n.declname if n.declname and emit_declname else ''
            # Resolve modifiers.
            # Wrap in parens to distinguish pointer to array and pointer to
            # function syntax.
            #
            for i, modifier in enumerate(modifiers):
                if isinstance(modifier, c_ast.ArrayDecl):
                    nstr += modifiers[i].prefix
                    if (i != 0 and isinstance(modifiers[i - 1], c_ast.PtrDecl)):
                        nstr = '(' + nstr + ')'
                    nstr += '[' + self.visit(modifier.dim) + ']'
                elif isinstance(modifier, c_ast.FuncDecl):
                    nstr += modifiers[i].prefix
                    if (i != 0 and isinstance(modifiers[i - 1], c_ast.PtrDecl)):
                        nstr = '(' + nstr + ')'
                    nstr += '(' + self.visit(modifier.args) + ')'
                elif isinstance(modifier, c_ast.PtrDecl):
                    if modifier.quals:
                        nstr = modifiers[i].prefix + \
                              '* %s%s' % (' '.join(modifier.quals), nstr)
                    else:
                        nstr = modifiers[i].prefix + '*' + nstr
            if nstr: s += nstr
            return s
        elif typ == c_ast.Decl:
            return self._generate_decl(n.type)
        elif typ == c_ast.Typename:
            return self._generate_type(n.type, no_type=no_type)
        elif typ == c_ast.IdentifierType:
            return ' '.join(n.names)
        elif typ in (c_ast.ArrayDecl, c_ast.PtrDecl, c_ast.FuncDecl):
            return self._generate_type(n.type, modifiers + [n], no_type=no_type)
        else:
            return self.visit(n)
