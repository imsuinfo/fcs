After lots of frustration and not-close-enough results with other HTML to PDF modules & libraries, I decided to make a Drupal implementation of something I know works extremely well.

IMHO: WkHtmlToPdf is just fantastic and works much better than things like mPDF.

The Print module was way more than I needed and still would have required custom work for my requirements.

**This is intended for module developers, nothing is usable from the front-end.**

This module is a Drupal Library implementation for the PHPWkHtmlToPdf wrapper and as a result is extremely small because it uses WkHtmlToPdf to do the heavy work.

## Quick Start

The following example is an adaption from the official PhpWkHtmlToPdf docs located at: https://github.com/mikehaertl/phpwkhtmltopdf

```php
if (($library = libraries_load('phpwkhtmltopdf')) && !empty($library['loaded'])) {
    $pdf = new WkHtmlToPdf;

    // Add a HTML file, a HTML string or a page from a URL
    $pdf->addPage('/home/joe/page.html');
    $pdf->addPage('<html>....</html>');
    $pdf->addPage('http://google.com');

    // Add a cover (same sources as above are possible)
    $pdf->addCover('mycover.html');

    // Save the PDF
    $pdf->saveAs('/tmp/new.pdf');

    // ... or send to client for inline display
    $pdf->send();

    // ... or send to client as file download
    $pdf->send('test.pdf');
}
```

## Requirements

### WkHtmlToPdf Installed
You must have WkHtmlToPdf installed and working on the server before attempting to use this wrapper library.
See https://github.com/wkhtmltopdf/wkhtmltopdf for installation details if my unofficial instructions don't work for you.

#### TLDR
*Basically you'll want to download the latest release, untar, put it somewhere appropriate, then symlink the `[wkhtmltox untarred path]/bin/wkhtmltopdf` to `/usr/bin/wkhtmltopdf`.*

### PhpWkHtmlToPdf Library
You will need to make sure the PhpWkHtmlToPdf library has been installed in the sites/all/library/phpwkhtmltopdf folder. See Step 2 of my install instructions below.

## Installation

Please note: these are my own instructions for my install on Ubuntu 12.04.2 LTS 64-bit.

### Step 1 - (My Unofficial) WkHtmlToPdf Install Instructions
```bash
cd ~
wget https://github.com/wkhtmltopdf/wkhtmltopdf/releases/download/0.12.0/wkhtmltox-linux-amd64_0.12.0-03c001d.tar.xz
tar -xJf wkhtmltox-linux-amd64_0.12.0-03c001d.tar.xz
sudo mv wkhtmltox /etc/wkhtmltox
sudo ln -s /etc/wkhtmltox/bin/wkhtmltopdf /usr/bin/wkhtmltopdf
```

*See https://github.com/wkhtmltopdf/wkhtmltopdf/blob/master/INSTALL.md for more detailed instructions*

### Step 2 - Install PhpWkHtmlToPdf to Library
```bash
cd [siteroot]/sites/all
wget https://github.com/mikehaertl/phpwkhtmltopdf/archive/1.2.1.tar.gz
tar -zxf 1.2.1.tar.gz
mv phpwkhtmltopdf-* phpwkhtmltopdf
```

You basically want to make sure the file is in the following path:
 `[siteroot]/sites/all/libraries/phpwkhtmltopdf/WkHtmlToPdf.php`

### Step 3 - Enable this Module
Then install/enable this module like any other Drupal module.

## Credits

A very special thanks goes out to the maintainers of WkHtmlToPDF and to authors of the PHP wrapper.

See WkHtmlTpPDF Contributors:
https://github.com/wkhtmltopdf/wkhtmltopdf/graphs/contributors

See PHP Wrapper Contributors:
https://github.com/mikehaertl/phpwkhtmltopdf/graphs/contributors