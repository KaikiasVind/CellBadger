# Badger - version 0.1

## General workflow
- Input of single Cell gene expression dataset(s) produced by CellRanger (relative path: "/analysis/diffexp/graphclust/differential_expression.csv")
  with input format: CSV - unmodified as given from cell Ranger output
- Input of cell / tissue-marker file
  with input format: CSV - An explanatory illustration can be found below
- Run project (the marker file and the dataset files are parsed and correlated clusterwise to the cells / tissues in the given marker file)
- If given - the different datasets are being shown as separate tabs
- The table in the upper part of the window shows the top hits from type mapping found with the given marker file
- The table on the bottom shows the gene expression for the given clusters and can be filtered via the line edit and sorted after single gene expression counts.

Accepted format for CSV marker files with delimiter "," :

Gene name | Gene ID | Cell / tissue-type | Cell / tissue-type |  ..  |
| ------ | ------ | ------ | ------ |  ------  |
| GENE NAME | GENE ID | Gene expression count | Gene expression count |  ..  |
| GENE NAME | GENE ID | Gene expression count | Gene expression count |  ..  |
|    ..   |    ..   |          ..           |          ..           |  ..  |

## Known bugs
- The correlation method used so far doesn't seem to be sufficient enough to produce valid output, e.g. mapping to obviously wrong tissues with low affinity.
- Somewhat slow runtime. The algorithms used for correlation and for populating the tables are not efficient and therefore create computational bottlenecks.
- Loading time: ~3s / given dataset

## Planned Features
#### Major features
- "Analysis tab" that shows the comparison between different datasets.
- Refinement of the correlation method (e.g. with prioritization of markers with [PanglaoDB](https://panglaodb.se/markers.html?cell_type=%27all_cells%27) and usage of other base cell-marker data specialized for expected tissues)
- Generalization of marker file input
- Reanalyzation with user-given values (e.g. different correlation method, different cutoffs for gene expressions, different marker file, ...)

#### Default software-workflow features
- Uploading of new datasets / cell-marker files after initial software startup
- Implementing a project file that can be used to save and load the current project
- Exporting of certain data (e.g. selected gene expressions in certain clusters)

## Project structure
- An illustration that explains the basic code structure will be provided later on