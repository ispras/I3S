#-----------------------------------------------------------------
# _ast_gen.py
#
# Generates the AST Node classes from a specification given in
# a configuration file
#
# The design of this module was inspired by astgen.py from the
# Python 2.5 code-base.
#
# Eli Bendersky [https://eli.thegreenplace.net/]
# License: BSD
#-----------------------------------------------------------------
import pprint
from string import Template


class ASTCodeGenerator(object):
    def __init__(self, cfg_filename='_c_ast.cfg'):
        """ Initialize the code generator from a configuration
            file.
        """
        self.cfg_filename = cfg_filename
        self.node_cfg = [NodeCfg(name, contents)
            for (name, contents) in self.parse_cfgfile(cfg_filename)]

    def generate(self, file=None):
        """ Generates the code into file, an open file buffer.
        """
        src = Template(_PROLOGUE_COMMENT).substitute(
            cfg_filename=self.cfg_filename)

        src += _PROLOGUE_CODE
        for node_cfg in self.node_cfg:
            src += node_cfg.generate_source() + '\n\n'

        file.write(src)

    def parse_cfgfile(self, filename):
        """ Parse the configuration file and yield pairs of
            (name, contents) for each node.
        """
        with open(filename, "r") as f:
            for line in f:
                line = line.strip()
                if not line or line.startswith('#'):
                    continue
                colon_i = line.find(':')
                lbracket_i = line.find('[')
                rbracket_i = line.find(']')
                if colon_i < 1 or lbracket_i <= colon_i or rbracket_i <= lbracket_i:
                    raise RuntimeError("Invalid line in %s:\n%s\n" % (filename, line))

                name = line[:colon_i]
                val = line[lbracket_i + 1:rbracket_i]
                vallist = [v.strip() for v in val.split(',')] if val else []
                yield name, vallist


class NodeCfg(object):
    """ Node configuration.

        name: node name
        contents: a list of contents - attributes and child nodes
        See comment at the top of the configuration file for details.
    """

    def __init__(self, name, contents):
        self.name = name
        self.all_entries = []
        self.attr = []
        self.child = []
        self.seq_child = []

        for entry in contents:
            clean_entry = entry.rstrip('*')
            self.all_entries.append(clean_entry)

            if entry.endswith('**'):
                self.seq_child.append(clean_entry)
            elif entry.endswith('*'):
                self.child.append(clean_entry)
            else:
                self.attr.append(entry)

    def generate_source(self):
        src = self._gen_init()
        src += '\n' + self._gen_children()
        src += '\n' + self._gen_iter()

        src += '\n' + self._gen_attr_names()
        return src

    def _gen_init(self):
        src = "class %s(Node):\n" % self.name

        if self.all_entries:
            args = ', '.join(self.all_entries)
            slots = ', '.join("'{0}'".format(e) for e in self.all_entries)
            if self.name == 'ID':
                slots += ", 'coord', 'prefix', 'var_type', '__weakref__'"
                arglist = "(self, %s, coord=None, prefix='', var_type=None)" % args
            elif self.name == 'FuncCall':
                slots += ", 'coord', 'prefix', 'args_type', '__weakref__'"
                arglist = "(self, %s, coord=None, prefix='', args_type=None)" % args
            elif self.name == 'FileAST':
                slots += ", 'coord', 'prefix', 'suffix', '__weakref__'"
                arglist = "(self, %s, coord=None, prefix='', suffix='')" % args
            else:
                if self.name == 'IdentifierType':
                    slots += ", 'coord', 'prefix', 'is_local_tcg', '__weakref__'"
                else:
                    slots += ", 'coord', 'prefix', '__weakref__'"

                if self.name == 'Compound':
                    arglist = "(self, %s, coord=None, prefix=['',''])" % args
                else:
                    arglist = "(self, %s, coord=None, prefix='')" % args
        else:
            slots = "'coord', 'prefix', '__weakref__'"
            arglist = "(self, coord=None, prefix='')"

        src += "    __slots__ = (%s)\n" % slots
        src += "    def __init__%s:\n" % arglist

        for name in self.all_entries + ['coord', 'prefix']:
            src += "        self.%s = %s\n" % (name, name)

        if self.name == 'ID':
            src += "        self.var_type = var_type\n"
        elif self.name == 'FuncCall':
            src += "        self.args_type = args_type\n"
        elif self.name == 'IdentifierType':
            src += "        self.is_local_tcg = False\n"
        elif self.name == 'FileAST':
            src += "        self.suffix = suffix\n"

        return src

    def _gen_children(self):
        src = '    def children(self):\n'

        if self.all_entries:
            src += '        nodelist = []\n'

            for child in self.child:
                src += (
                    '        if self.%(child)s is not None:' +
                    ' nodelist.append(("%(child)s", self.%(child)s))\n') % (
                        dict(child=child))

            for seq_child in self.seq_child:
                src += (
                    '        for i, child in enumerate(self.%(child)s or []):\n'
                    '            nodelist.append(("%(child)s[%%d]" %% i, child))\n') % (
                        dict(child=seq_child))

            src += '        return tuple(nodelist)\n'
        else:
            src += '        return ()\n'

        return src

    def _gen_iter(self):
        src = '    def __iter__(self):\n'

        if self.all_entries:
            for child in self.child:
                src += (
                    '        if self.%(child)s is not None:\n' +
                    '            yield self.%(child)s\n') % (dict(child=child))

            for seq_child in self.seq_child:
                src += (
                    '        for child in (self.%(child)s or []):\n'
                    '            yield child\n') % (dict(child=seq_child))

            if not (self.child or self.seq_child):
                # Empty generator
                src += (
                    '        return\n' +
                    '        yield\n')
        else:
            # Empty generator
            src += (
                '        return\n' +
                '        yield\n')

        return src

    def _gen_attr_names(self):
        src = "    attr_names = (" + ''.join("%r, " % nm for nm in self.attr) + ')'
        return src


_PROLOGUE_COMMENT = \
r'''#-----------------------------------------------------------------
# ** ATTENTION **
# This code was automatically generated from the file:
# $cfg_filename
#
# Do not modify it directly. Modify the configuration file and
# run the generator again.
# ** ** *** ** **
#
# pycparser: c_ast.py
#
# AST Node classes.
#
# Eli Bendersky [https://eli.thegreenplace.net/]
# License: BSD
#-----------------------------------------------------------------

'''

_PROLOGUE_CODE = r'''
import sys

def _repr(obj):
    """
    Get the representation of an object, with dedicated pprint-like format for lists.
    """
    if isinstance(obj, list):
        return '[' + (',\n '.join((_repr(e).replace('\n', '\n ') for e in obj))) + '\n]'
    else:
        return repr(obj) 

def adapt_prefix(s):
    if sys.version_info[0] == 2:
        s = s.decode("utf-8").replace(" ", u"\u2423").encode("utf-8")
    else:
        s = s.replace(" ", u"\u2423")
    # \\l - for left justification
    return s.\
        replace("\\", "\\\\").\
        replace('\n', "\\\\n\\l").\
        replace("\t", "\\\\t")

class Node(object):
    __slots__ = ()
    """ Abstract base class for AST nodes.
    """
    def __repr__(self):
        """ Generates a python representation of the current node
        """
        result = self.__class__.__name__ + '('
        
        indent = ''
        separator = ''
        for name in self.__slots__[:-2]:
            result += separator
            result += indent
            result += name + '=' + (_repr(getattr(self, name)).replace('\n', '\n  ' + (' ' * (len(name) + len(self.__class__.__name__)))))
            
            separator = ','
            indent = '\n ' + (' ' * len(self.__class__.__name__))
        
        result += indent + ')'
        
        return result

    def children(self):
        """ A sequence of all children that are Nodes
        """
        pass

    def show(self,
        buf=sys.stdout,
        offset=0,
        attrnames=False,
        nodenames=False,
        showcoord=False,
        showprefix=False,
        showtype=False,
        _my_node_name=None
    ):
        """ Pretty print the Node and all its attributes and
            children (recursively) to a buffer.

            buf:
                Open IO buffer into which the Node is printed.

            offset:
                Initial offset (amount of leading spaces)

            attrnames:
                True if you want to see the attribute names in
                name=value pairs. False to only see the values.

            nodenames:
                True if you want to see the actual node names
                within their parents.

            showcoord:
                Do you want the coordinates of each Node to be
                displayed.

            showprefix:
                Do you want the prefix of each Node to be
                displayed.

            showtype:
                Do you want the type of each ID to be displayed.
        """
        lead = ' ' * offset
        if nodenames and _my_node_name is not None:
            buf.write(lead + self.__class__.__name__+ ' <' + _my_node_name + '>: ')
        else:
            buf.write(lead + self.__class__.__name__+ ': ')

        if self.attr_names:
            if attrnames:
                nvlist = [(n, getattr(self,n)) for n in self.attr_names]
                attrstr = ', '.join('%s=%s' % nv for nv in nvlist)
            else:
                vlist = [getattr(self, n) for n in self.attr_names]
                attrstr = ', '.join('%s' % v for v in vlist)
            buf.write(attrstr)

        if showcoord:
            buf.write(' (at %s)' % self.coord)

        if showprefix:
            prefix = self.prefix
            if isinstance(prefix, list):
                buf.write(' (prefix = ')
                for e in prefix:
                    buf.write('[%s]' % adapt_prefix(e))
            else:
                buf.write(' (prefix = %s)' % adapt_prefix(prefix))

        if showtype:
            if isinstance(self, ID):
                buf.write('(type =')
                if self.var_type is None:
                    buf.write(' undefine')
                else:
                    for e in self.var_type:
                        buf.write(' %s' % e)
                buf.write(')')
        buf.write('\n')

        for (child_name, child) in self.children():
            child.show(
                buf,
                offset=offset + 2,
                attrnames=attrnames,
                nodenames=nodenames,
                showcoord=showcoord,
                showprefix=showprefix,
                showtype=showtype,
                _my_node_name=child_name)

    def determine_var_type(self, ns = {}, func_params = {}):
        """ Determine variable (ID) type.

            ns is dictonary containing current name space:
                Key: ID name
                Value: [ID type, is_local]
        """

        if isinstance(
            self,
            (Label, Goto, DoWhile, While, Case, Default)
        ):
            for v in ns.values():
                v[1] = True
            for child_name, child in self.children():
                child.determine_var_type(ns, func_params)

        elif isinstance(self, FuncCall):
            for v in ns.values():
                v[1] = True
            if self.name.name in func_params:
                self.args_type = func_params[self.name.name]

            self.args.determine_var_type(ns, func_params)

        elif isinstance(self, For):
            if self.init is not None:
                self.init.determine_var_type(ns, func_params)
            for v in ns.values():
                v[1] = True
            if self.cond is not None:
                self.cond.determine_var_type(ns, func_params)
            if self.next is not None:
                self.next.determine_var_type(ns, func_params)
            self.stmt.determine_var_type(ns, func_params)

        elif isinstance(self, If):
            self.cond.determine_var_type(ns, func_params)
            for v in ns.values():
                v[1] = True
            self.iftrue.determine_var_type(ns, func_params)
            if self.iffalse is not None:
                self.iffalse.determine_var_type(ns, func_params)

        elif isinstance(self, Compound):
            if self.block_items is not None:
                # We don't need to deepcopy namespace.
                # If new variable has been declared,
                # list link will be changed to new.
                # If variable has been used,
                # value in all parent name space will be updated.
                ns = ns.copy()
                for child in self.block_items:
                    child.determine_var_type(ns, func_params)

        elif isinstance(self, FuncDef):
            func_name = self.decl.type.type.declname
            func_params[func_name] = []
            body_ns = ns.copy()
            for p in self.decl.type.args.params:
                while not (isinstance(p, TypeDecl) and isinstance(p.type, IdentifierType)):
                    p = p.type
                body_ns[p.declname] = [p.type, False]
                func_params[func_name].append(p.type.names)

            self.body.determine_var_type(body_ns)

        elif isinstance(self, TypeDecl) \
         and isinstance(self.type, IdentifierType):
            # TODO: support typedef
            ns[self.declname] = [self.type, False]

        elif isinstance(self, ID):
            if self.name in ns:
                id_desc = ns[self.name]
                self.var_type = id_desc[0].names
                if id_desc[1]:
                    l = self.var_type
                    if 'tcg' in l:
                        id_desc[0].is_local_tcg = True

        else:
            for child_name, child in self.children():
                child.determine_var_type(ns, func_params)


class NodeVisitor(object):
    """ A base NodeVisitor class for visiting c_ast nodes.
        Subclass it and define your own visit_XXX methods, where
        XXX is the class name you want to visit with these
        methods.

        For example:

        class ConstantVisitor(NodeVisitor):
            def __init__(self):
                self.values = []

            def visit_Constant(self, node):
                self.values.append(node.value)

        Creates a list of values of all the constant nodes
        encountered below the given node. To use it:

        cv = ConstantVisitor()
        cv.visit(node)

        Notes:

        *   generic_visit() will be called for AST nodes for which
            no visit_XXX method was defined.
        *   The children of nodes for which a visit_XXX was
            defined will not be visited - if you need this, call
            generic_visit() on the node.
            You can use:
                NodeVisitor.generic_visit(self, node)
        *   Modeled after Python's own AST visiting facilities
            (the ast module of Python 3.0)
    """

    _method_cache = None

    def visit(self, node):
        """ Visit a node.
        """

        if self._method_cache is None:
            self._method_cache = {}

        visitor = self._method_cache.get(node.__class__.__name__, None)
        if visitor is None:
            method = 'visit_' + node.__class__.__name__
            visitor = getattr(self, method, self.generic_visit)
            self._method_cache[node.__class__.__name__] = visitor

        return visitor(node)

    def generic_visit(self, node):
        """ Called if no explicit visitor function exists for a
            node. Implements preorder visiting of the node.
        """
        for c in node:
            self.visit(c)

'''
