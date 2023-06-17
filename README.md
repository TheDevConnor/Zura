# Azura Lang
<!-- Add in an image of the mascote -->
<p align="center">
  <img src="images/azura_mascote_trimmed.png" alt="Azura Lang Mascote" width="200" height="200">
</p>

Azura Lang is a high-level programming language designed to be expressive, versatile, and easy to use. It offers dynamic typing with strong type inference, allowing developers to write code without explicitly specifying types. Azura Lang supports multiple paradigms, including procedural and object-oriented programming.
Here is the Discord server! [Azura Lang Discord](https://discord.gg/JvExQpGuXM)
## Features

- **High-Level**: Azura Lang provides a high-level abstraction, allowing developers to write code that is closer to human-readable language.
- **Dynamic Typing**: Variables in Azura Lang are dynamically typed, meaning their types are determined at runtime.
- **Strong Type Inference**: Azura Lang employs strong type inference, automatically deducing the types of variables based on their assigned values.
- **File Extension**: Azura Lang code files have the extension `.az`.
- **Syntax Highlighting**: Here is the link for the syntax highlighting! [Azura Lang Syntax Highlighting](https://github.com/TheDevConnor/Azura-Syntax-Highlighting)

## Installation

To start using Azura Lang, follow the installation instructions below:

1. Download the latest version of the Azura Lang interpreter from the official Azura Lang website.
2. Install the interpreter according to the instructions provided for your operating system.
3. Verify the installation by opening a terminal or command prompt and running the following command:

```
azura.exe --version
```

If the command prints the version information, the installation was successful.

## Usage

To run Azura Lang code, follow the steps below:

1. Create a new file with the `.az` extension (e.g., `example.az`).
2. Write your Azura code in the file using a text editor of your choice.
3. Open a terminal or command prompt and navigate to the directory where the Azura code file is located.
4. Run the Azura code using the following command:

```
azura.exe your_file_name.az
```
For a list of all available commands and options, you can run the following command:

```
azura.exe --help
```

## Examples

Here are some examples showcasing the features and syntax of Azura Lang:

### Fibonacci Sequence

```
func fib(n) {
    return (1 / sqrt(5)) * ( pow(((1 + sqrt(5)) / 2), n) - pow(((1 - sqrt(5)) / 2), n) );
}

info "Fibonacci closed formula:";
have start := clock();
info fib(30);
info clock() - start;

func fib_re(n) {
    if (n < 2) {
        return n;
    }

    return fib_re(n - 1) + fib_re(n - 2);
}

info "Fibonacci recursive:";
have start := clock();
info fib_re(30);
info clock() - start;
```

### Variable Assignment and Inheritance
```
have a := "a";
have b := "b";
have c := "c";

// Assignment is right-associative
a = b = c;
info a; // expect: c
info b; // expect: c
info c; // expect: c

class Math {
    add(a, b) { return a + b; }
    subtract(a, b) { return a - b; }
}

have math := Math();
info math.add(10, 3); // expect: 13
info math.subtract(10, 3); // expect: 7
```

### Shape Classes and Inheritance
```
class Math {
    multiply(a, b) { return a * b; }
}

class Circle -> Math {
    init(radius) { this.radius = radius; }

    area() { return this.multiply(this.radius, this.radius) * pi(); }
}

have circle := Circle(10);
info "Circle area:";
info circle.area(); // expect: 314.159
```

### Loop Examples

For Loop
```
for (i = 0; i < 10; i = i + 1) {
    info i;
}
```
While Loop
```
have x := 0;
while (x < 10) {
    info x;
    x = x + 1;
}
```
If-Else Statement
```
if (x > 3) {
    info x;
} else {
    info x;
}

// Which can also be written as:
if (x > 3) info x;
else info x;

// Or even:
if (x > 3) info x; else info x;
```

### Function Declarations
```
// Function that calculates the factorial of a number
func factorial(n) {
    if (n == 0) {
        return 1;
    }

    return n * factorial(n - 1);
}

// Function that calculates the power of a number
func power(base, exponent) {
    if (exponent == 0) {
        return 1;
    }

    return base * power(base, exponent - 1);
}

// Usage example
have x := 5;
have y := 3;

info "Factorial of";
info x;
info factorial(x);
info "Power of";
info x;
info power(x, y);
```

## Error Handling Example
```
[line: 8] [pos: 12] Error at ';'
        info ;
            ^^^
Expected expression!

[line: 13] [pos: 10] Error at '='
have timer = ScopeTimer();
          ^^^
For assining a variable use the operation of ':='.
For example 'have add := 45.2 + 2;'
```
## Contributing
Contributions to Azura Lang are welcome! If you find any bugs, have suggestions for improvements, or want to contribute new features, please submit a pull request.

## License
Azura Lang is licensed under the MIT License. Feel free to use, modify, and distribute the language for personal or commercial projects.

We hope you find Azura Lang useful and enjoyable to work with. If you have any questions or need further assistance, please don't hesitate to reach out.

Happy coding!

