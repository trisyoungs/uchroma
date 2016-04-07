---
title: Collections
brief: Managing collections
taxonomy:
  category: docs
docroot: /uchroma/docs
header_class: alt
---

**uChroma** allows many collections in a single session, each viewed on its own set of axes if necessary. Moreover each collection can have one or more sub-collections (for instance fitted data, or extracted slices), creating a hierarchy of data. The entire hierarchy is viewable in tree form on the collections side-bar:

Each collection in the list has associated with it an icon representing its principal type: a master collection (which can only exist as a 'top level' item in the list), fit data (which is derived through fitting an equation to another collection), and extracted data (derived from taking slices through a given direction in another collection).  The icons are as follows:

XXX TODO

If the collection has a small red 'no' sign in the bottom-right corner, this indicates that this particular set of data is not display on any available pane in the main view. This can be remedied through the Collection Context Menu (see below)</p>

## The Collection Context Menu

The context menu of the collection list provides quick access to useful collection-specific functions, such as generating/updating fits, removing unwanted data, or moving collections between panes in the main view. The available functions are as follows:

| Item | Action |
|------|--------|
| **Delete** | Delete the collection |
| **Duplicate** | Duplicate the collection |
| **Move to...** | Move the collection to a different pane |
| **New Fit Equation** Fit a new equation to the collection's data |
| **Edit Fit Equation** | Edit the fit equation stored in the collection |
| **Update/Continue Fit** | Refit the equation stored in the collection |
