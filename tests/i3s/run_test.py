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

    ast = parse_file("i3s.c")
    i3s_processing.convert_i3s_to_c(ast)
    generator = c_generator.CGenerator()
    i3s_processed = generator.visit(ast)

    TestCase().assertEqual(i3s_processed + ast.suffix, reference_standards)

#------------------------------------------------------------------------------
if __name__ == "__main__":
    i3s_test()
    print("Test was passed")
