---
title: Typesetting
brief: Advanced formatting for textual aspects of graphs
taxonomy:
  category: docs
docroot: /uchroma/docs
header_class: alt
---

It is possible to apply various simple typesetting to axis labels, titles etc. through the use of simple TeX-style commands within the text of the label itself. These commands can be used in any text displayed anywhere on a graph area, including number formats, axis titles, additional labels etc. All commands begin with a backslash ('\') followed by a short character sequence identifying the command, and ended with a pair of curly braces ('{' and '}'). The text which is to be the target of the formatting is enclosed in the curly braces, and such sequences can be nested (i.e. enclosed within other curly braces).

## Typesetting Commands

| Command | Effect |
|---------|--------|
| \b{text} | The enclosed 'text' is displayed in bold |
| \it{text} | The enclosed 'text' is displayed in italics |
| \sub{text} | The enclosed 'text' is lowered by one 'level' of subscript |
| \sup{text} | The enclosed 'text' is raised by one 'level' of superscript |

## Examples

XXX TODO
