---
title: Data
brief: What uChroma calls it, and how it manages it
taxonomy:
  category: docs
docroot: /uchroma/docs
header_class: alt
---

In most graphing / visualisation packages, a single dataset (XY data, curve, trace, line - call it what you will) exists independently of the others. In **uChroma** a number of datasets are grouped together into a _collection_. The individual datasets may still be displayed as individual line plots, or may be used to generate a continuous surface (see [collection styling](/uchroma/docs/style)) but they are all displayed at the same time as each other. In some sense this complicates the use of uChroma as a 2D graphing package, but permits the facile creation of 3D dataset and the interrogation thereof.

Multiple collections can exist simultaneously in a single _session_, and each collection may contain sub-collections in the form of fitted or extracted data. All collections in the current session can be viewed through the [collections list](/uchroma/docs/collections).

