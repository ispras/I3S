from argparse import (
    ArgumentParser
)
from pycparser import (
    parse_file,
    c_generator,
    i3s_processing
)

def main():
    parser = ArgumentParser()


    parser.add_argument("--in-file", required = True)
    parser.add_argument("--out-file", required = True)
    parser.add_argument("--out-astfile")
    parser.add_argument("-d", "--debug", action = "store_true")

    arguments = parser.parse_args()

    ast = parse_file(arguments.in_file)
    i3s_processing.convert_i3s_to_c(ast, arguments.debug)
    generator = c_generator.CGenerator()
    out_file = open(arguments.out_file, "w")
    i3s_processed = generator.visit(ast)
    out_file.write(i3s_processed)
    out_file.write(ast.suffix + "\n")
    out_file.close()

    if arguments.out_astfile is not None:
        out_astfile = open(arguments.out_astfile, "w")
        ast.show(
            buf = out_astfile,
            attrnames = False,
            showcoord = False,
            showprefix = True,
            showtype = True
        )
        out_astfile.close()

    print(arguments.in_file + " was translated to " + arguments.out_file)


if __name__ == '__main__':
    main()
