# Magna

Magna is designed to become a simple programming language with an unique but fast and flexible approach to parsing expressions.
The project in its infancy.

Recomendations for commiters:
- Linux kernel coding style is a preffered coding style;
- C89 is the main language standart used within Magna;
- Avoid using recursions as much as possible;
- Headers must also include main .c files if built without -DMAGNA_MODULE.

Current version uses mixed notation, for example:
input: "10 + + 1 ((* + 2 3 4) + 5 * 6) 7 *"
outputs:
2 + 3 = 5
5 * 4 = 20
5 * 6 = 30
20 + 30 = 50                                                                     1 + 50 = 51
51 * 7 = 357
10 + 357 = 367
Sucess.

Prefix expressions are solved before postfix expressions.
