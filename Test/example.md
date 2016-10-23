
# Supported Markdown Features

## Horizontal Rules

Lines of asterisks (\*), hyphens (\-), or underscores (\-) are interpreted as a horizontal rule.

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
