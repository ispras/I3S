# I3S (Instruction Set Semantics Specification) Translator

The goal of this project is to simplify instruction set semantics specification
generation for QEMU using high-level language (I3S).

## I3S overview

I3S is C99 extension.
I3S is used to avoid using low-level TCG API.
More about TCG you can read in *qemu/tcg/README*.

I3S contain new type `tcg`.
This type support several type specifiers: `short` and `long`.
By default `tcg` is `unsigned`, but you can use `signed` to create signed
variable.
Consider the possible types:
- `tcg`: target long run-time variable (equivalent `TCGv`);
- `long tcg`: 64-bits run-time variable (equivalent `TCGv_i64`);
- `short tcg`: 32-bits run-time variable (equivalent `TCGv_i32`).

A user can work with `tcg` variable like with other C variable with some
specificity:
- for `tcg` vars was added left and right rotate shifts (`<<<`, `>>>`);
- if one of the var in expression is `unsigned` the result is `unsigned` too;
- it is forbidden to assign `tcg` vars to `non-tcg`;
- for `tcg` vars applicable rules like integer conversion rules,
but the sequence of priorities is as follows:
`short tcg` < `long tcg` < `tcg`.
- if expression contains `tcg` var and C var the signedness determine by `tcg` var.

### Implementation constraints:
- Doesn't use *tcg* var in return expression.
- Does't support *tcg* var pointer.
- Global *tcg* var must be only extern.
- Loop *for* is *tcg* dependent if the condition is *tcg* dependent.

ATTENTION: Try to avoid declare variables with the same name, because the
compound will be converted to linear code without compound
for tcg-depended `if/loops`.
The tool only ensure that variable in tcg-depended `if/loops` will be renamed
if its name has been encountered before.
Renaming is done by adding a suffix to the name.
A message with a new name of vars and line number in src file will be displayed
in console after renaming.
Also renaming don't work for declaration in init in 'For' loop.

To become familiar with all restrictions you need to read TODO.

### TODO:
- Array:

    - support tcg index (by automatic helpers generation);
    - fixup pointer handling: if a[n][k] has tcg type ==> a[n] has tcg type too (a[n] should be a pointer).

- Analysis functions body and arguments for specific *tcg* expressions.

- Support *tcg* fields in Struct and Union.

- Warning when *tcg* var dependent if/loops contains *C* vars.

- Trace basic block only for *tcg* var.

- Support special *tcg* conditional Always/Never (True/False).

- Support ternary operator.

- Support if/loops without compounds that contains tcg-expression,
because *tcg* expression can deploy to several *C* expressions.

- Integrate `gen_set_label` sequences in one.

- Check matching of automatically generated var names with vars in
specification.

- Take into account the typedef *tcg* type.

- Reuse *tcg* var after free: doesn't declare new var, just make *tcg_temp_new*.


## Getting started

The tool is written in Python.
Both 2.7.3+ and 3.3+ versions are supported.

### How to run
`python i3s_to_c.py --in-file i3s_in.c --out-file tcg_out.c`

If you want to view AST you can use `--out-astfile ast.txt`.

Use `--debug` or `-d` to run I3S in debug mode.
In this mode tool automatically add comments with the original I3S language
constructs to generated file.

### Run test
`python run_test.py`

Test locate in `\test\i3s`.

The essence of the test is to compare the generated from the *i3s.c* new
code with code that was generated before tools modification.

The reference standards is stored in *i3s_reference_standards.c*.

You can update *i3s_reference_standards.c* using

`python update_reference_standards.py`
