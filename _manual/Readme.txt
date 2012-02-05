Github does not support jekyll generated 404 pages,
but requires a 404.html in the root folder.
To despite have a nice 404.html it is generated
from 404.md in _manual manually:
Regenerate if something relevant for 404.html changed
(either content itself, or changes in jekyll-bootstrap):

1. Copy 404.md to ../
2. Generate using jekyll 
3. Copy resulting _site/404.html to ../404.html
4. Remove ../404.md
5. Push gh-pages branch. (as for all updates)

Note that 404.md links must start with "/WikiOnBoard/"