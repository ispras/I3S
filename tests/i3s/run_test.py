import sys
from unittest import (
    TestCase,
    main
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


class TestGeneration(TestCase):

    def generation(self, refs_filename, debug):
        with open(refs_filename, "r") as f:
            reference_standards = f.read()

        generator = c_generator.CGenerator()

        ast = parse_file("i3s.c")
        i3s_processing.convert_i3s_to_c(ast, debug)
        i3s_processed = generator.visit(ast)
        self.assertEqual(i3s_processed + ast.suffix, reference_standards)

    def test_generation_without_debug(self):
        self.generation("i3s_reference_standards.c", False)

    def test_generation_with_debug(self):
        self.generation("i3s_debug_reference_standards.c", True)


if __name__ == "__main__":
    main()
