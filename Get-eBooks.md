---
layout: page
title: Get-eBooks
header : Get-eBooks
group: navigation
---

WikiOnBoard is a viewer for [zim files](http://openzim.org), a file format for wiki content supported by the Wikimedia 
foundation. 

# Where to get eBooks?
Places where you can find the zim-Files, including the Wikipedia, for download

* [kiwix.org](http://kiwix.org) has Wikipedia-Versions in many different languages. 

* [openzim.org](http://openzim.org/ZIM_File_Archive)

* In the wikipedia you can use "Print/export", "Create a Book" to generate an eBook of selected pages. For use with WikiOnBoard you have to select "openZim" as format. See http://en.wikipedia.org/wiki/Help:Books for more information.

It is strongly recommend to download zim files on your desktop PC and copy them to the phone's memory card using the phones USB connection or using a memory card reader.
Downloading zim files directly on your phone is not recommended as the files are typically very large. Note that if the file is larger than 2 GB you definitely have to use a PC for downloading
as it is not possible to download it directly on the phone. 

# Handling of files larger than 2GB 

Unfortunately, current Symbian mobile phones do not support files which are larger than 2GB. To workaround this, you can split such files in chunks which are smaller than this limit. 

Do perform this you need a file splitter tool, on Windows you can use for example [FFSJ-Lite](http://www.jaist.ac.jp/~hoangle/filesj/).

Firstly, open the source zim file in the split tool and perform the split. 

Then you have to rename the generated files so that the first file has the extension `.zimaa`, the second `.zimab` and so on.

For example, if you split a file called `wikipedia.zim` with FFSJ-Lite you have to rename the generated files as follows: 

> `wikipedia.zim.001` must be renamed to `wikipedia.zimaa`, `wikipedia.zim.002` to `wikipedia.zimab` ,`wikipedia.zim.003` to `wikipedia.zimac`, and `wikipedia.zim.004` to `wikipedia.zimad`.

These files then must be stored on the memory card in a common folder. To open the wikipedia any of these files can be selected in the app. 
