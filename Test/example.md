
# Usage

The *umdoc* executable accepts the following command line arguments:

```
umdoc [<input-file>] [-o <output-file>] [-e <latex-engine>] 
      [-a <aux-directory>]
```

The default behavior of *umdoc* is to look in the working directory for a `umdoc.xml` file...


# Supported Markdown Features

## Titles

```
# Example Title

## Example Title Level 2

Example text.
```

results in:
```latexexample
# 1 ~ ~ ~Example Title {-}

## 1.1 ~ ~ ~Example Title Level 2 {-}

Example text.
```

## Cross References

This references to section~[](#horizontal-rules).

## Horizontal Rules {#horizontal-rules}

Lines of asterisks (\*), hyphens (\-), or underscores (\_) are interpreted as a horizontal rule.

```
***
```

or

```
*******
```

or

```
* * * * *
```

or

```
-------
```

or

```
______
```

results in:
```latexexample
___
```

## Inline Links

```
This is an [example link](https://github.com/craflin/umdoc).
```

results in:
```latexexample
This is an [example link](https://github.com/craflin/umdoc).
```

## Code Spans

```
Some text with code like `printf()`. Code span with backticks `` `test` ``.
```

results in:
```latexexample
Some text with code like `printf()`. Code span with backticks `` `test` ``.
```

## Bullet Lists

Lines starting with asterisk (\*), plus (\+), or hyphen (\-) are interpreted as bullet list items.

```
* item a
* item b
* item c
```

or

```
+ item a
+ item b
+ item c
```

or

```
- item a
- item b
- item c
```

results in:
```latexexample
- item a
- item b
- item c
```

List items can have multiple paragraphs.

```
* item a

  line 2

* item b
* item c
```

results in:
```latexexample
* item a

  line 2

* item b
* item c
```

Lists can be nested.

```
* item a
  * subitem a
  * subitem a
* item b
* item c
```

results in:
```latexexample
* item a
  * subitem a
  * subitem a
* item b
* item c
```

## Numbered Lists

```
1. item a
2. item b
3. item c
```

results in:
```latexexample
1. item a
2. item b
3. item c
```

```
1. item a
   1. subitem a
   2. subitem a
2. item b
3. item c
```

results in:
```latexexample
1. item a
   1. subitem a
   2. subitem a
2. item b
3. item c
```

## Inline Images

```
Text with inline an inline image like this ![](circle.png).
```

results in:
```latexexample
Text with inline an inline image like this ![](circle.png).
```

## Fenced Code Blocks

```c
#include <stdio.h>

int main(void)
{
    printf("Hello World\n");
    return 0;
}
```

```java
class HelloWorldApp {
    public static void main(String[] args) {
        System.out.println("Hello World!");
    }
}
```

```xml
<umdoc>
    <document>
        <pdf file="cover.pdf"/>
        <tableOfContents/>
        <newPage/>
        <md file="example.md"/>
    </document>
</umdoc>
```
