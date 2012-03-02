---
layout: page
title: Get-eBooks
header : Get-eBooks
group: navigation
---

WikiOnBoard is a viewer for [zim files](http://openzim.org), a file format for wiki content supported by the Wikimedia 
foundation. 

# Where to get eBooks?

Sites where you can find various zim files, including the Wikipedia

* [kiwix.org](http://kiwix.org) has Wikipedia-Versions in many different languages. 

* [openzim.org](http://openzim.org/ZIM_File_Archive)

* On the wikipedia website you can use "Print/export", "Create a Book" to generate an eBook of selected pages. For use with WikiOnBoard you have to select "openZim" as format.  [Here](http://en.wikipedia.org/wiki/Help:Books) you can find more information.

It is strongly recommended to download zim files to your desktop PC and copy them to the phone's memory card using the phones USB connection or a memory card reader.
Downloading zim files directly to your phone is not recommended as the files are typically very large. Note that if the file is larger than 2 GB, you definitely have to use a PC for downloading it
as it is not possible to download it directly to the phone. 

# Handling of files larger than 2GB 

Unfortunately, current Symbian mobile phones do not support files which are larger than 2GB. To workaround this, you can split these files into chunks of less than 2 GB each to
work around this issue. 

In order to do this, you will need a file splitter tool. On Windows you can use for example [FFSJ-Lite](http://www.jaist.ac.jp/~hoangle/filesj/).

First, open the source zim file in the split tool and perform the split. 

Then rename the generated files so that the first file has the extension `.zimaa`, the second `.zimab` and so on.

For example, if you split a file called `wikipedia.zim` with FFSJ-Lite, you have to rename the generated files as follows: 

> `wikipedia.zim.001` must be renamed to `wikipedia.zimaa`, `wikipedia.zim.002` to `wikipedia.zimab`, `wikipedia.zim.003` to `wikipedia.zimac`, and `wikipedia.zim.004` to `wikipedia.zimad`.

Then these files must be stored on the memory card in a common folder. To open the Wikipedia, any of these files can be selected in the app. 
