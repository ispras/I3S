import sys
from unittest import (
    TestCase
)

# This is not required if you've installed pycparser into
# your site-packages/ with setup.py
#
sys.path.extend(['.', '..', '../', '../..'])

from pycparser import (
    parse_file,
    c_generator,
    i3s_processing
)

def i3s_test():
    f_reference_standards = open("i3s_reference_standards.c", "r")
    reference_standards = f_reference_standards.read()
    f_reference_standards.close()

    f_reference_standards = open("i3s_debug_reference_standards.c", "r")
    d_reference_standards = f_reference_standards.read()
    f_reference_standards.close()
    print("Test without debug comments was passed")

    generator = c_generator.CGenerator()

    ast = parse_file("i3s.c")
    i3s_processing.convert_i3s_to_c(ast)
    i3s_processed = generator.visit(ast)
    TestCase().assertEqual(i3s_processed + ast.suffix, reference_standards)

    # debug comments generation testing
    ast = parse_file("i3s.c")
    i3s_processing.convert_i3s_to_c(ast, True)
    i3s_processed = generator.visit(ast)
    TestCase().assertEqual(i3s_processed + ast.suffix, d_reference_standards)
    print("Test with debug comments was passed")

#------------------------------------------------------------------------------
if __name__ == "__main__":
    i3s_test()
