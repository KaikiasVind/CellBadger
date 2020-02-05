# Badger - version 0.1

## General workflow
- Input of single Cell gene expression dataset(s) produced by CellRanger (relative path: "/analysis/diffexp/graphclust/differential_expression.csv")
  with input format: CSV - unmodified as given from cell Ranger output
- Input of cell / tissue-marker file
  with input format: CSV - One (An explanatory illustration will be provided later on)
- Run project (the marker file and the dataset files are parsed and correlated clusterwise to the cells / tissues in the given marker file)

## Known bugs
- Currently only works on Cell Ranger 3.0.2 - 5k_Pbmc dataset
- Table for cluster-wise gene epxression doesn't show the relative gene expression IDs but generalized ones -> Incorrect mapping

## Planned Features
### Major features
- "Analysis tab" that shows the comparison between different datasets.
- Generalization of marker file input
- Reanalyzation with user-given values (e.g. different correlation method, different cutoffs for gene expressions, different marker file, ...)

### Default software workflow features
- Uploading of new datasets / cell-marker files after initial software startup
- Implementing a project file that can be used to save and load the current project
- Exporting of certain data (e.g. selected gene expressions in certain clusters)

## Project structure
- (An illustration that explains the basic code structure will be provided later on)