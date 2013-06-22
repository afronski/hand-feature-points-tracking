Description:
================
*Title*: Hand feature points tracking in video sequences.

This is main repository of my master of science thesis. Solution is based on three algorithm: Dense Optical Flow, Sparse Optical Flow and tracker which use Randomized Trees approach (more in bibliography, file MainDocument.bib). Unfortunately, main document is written in polish.

All results and whole document (with graphics) is licensed by CC BY-NC-ND 3.0.
Source code is licensed on MIT License.

Technology List:
================

Domain logic:

  - C++ ( _ISO/IEC 14882:2003_ )
    - C++ Standard Library ( _iostream, string, ..._ )
    - OpenCV

Thin middle-end and simple front-end:

  - node.js ( _middle-end_ )
    - express
    - vid-streamer
    - colors
    - execSync
    - glob
  - pure JavaScript ( _ES5_ ), HTML5, CSS3 ( _front-end_ )
  - all visualizations and charts are based on d3.js

Continuous integration:

  - Python
    - SCons

Utilities:

  - FFmpeg ( _converting, transposing and recording videos_ )
  - Bash ( _automation scripts_ )
  - Git ( _VCS_ )
  - Sublime Text 2 ( _Editor_ )
  - LaTeX
