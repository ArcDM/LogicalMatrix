# LogicalMatrix
This project reads a logical statement and creates an intermediate class similar to an programmable logic array to contain it.
Multiple logical statements can be contained in the same structure without much extra space needed.
The LogicalMatrix can also be evaluated using given truth values of its identifiers.

The symbols `,` `(` `)` `&` `&&` `|` `||` `!` `AND` `OR` `NOT` are used to parse the logical statement and are reserved.
Any symbols not reserved can be used as identifiers with in a given logical statement.

Logical statement will all simplify to AND sets separated by ORs.
For example:    `( A | B ) & C`   will be displayed as    `A & C | B & C`

The Boolean algebra associativity, commutativity, distributivity, and order of operations are adhered to as expected.

The internal data structure does not simplify complement operations such as `A & !A` nor `A | !A` but the result will still evaluate the same.
