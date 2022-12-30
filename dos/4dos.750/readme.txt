
                             4DOS v7.50

                     README.TXT -- August 2004

                          JP Software Inc.
                            P.O. Box 328
                     Chestertown, MD 21620, USA

                           (410) 810-8818
                         fax (410) 810-0026
                       Email sales@jpsoft.com
                        Web http://jpsoft.com


Greetings, and thanks for trying 4DOS!

This file contains a variety of information you should read before using
4DOS, including:

      Installation Notes
      Files Included with this Version
      Technical Support and Downloads
      Legal Notices


INSTALLATION NOTES
------------------

   4DOS is distributed as an EXE file which installs 4DOS when run.  The
   EXE file is "ZIP-compatible" (i.e., it can also be read as .ZIP files).
      
   If you have unzipped a distribution .EXE file yourself, or are using
   a CD copy, and prefer to perform the installation manually, see the 
   Introduction and Installation Guide in the file INTRO.TXT.  The 
   detailed instructions for 4DOS are in Chapter 5.

   If you unzip the distribution EXE file manually, you will find many \
   extra files which are used by the installer and are not part of the 
   product itself.  You can use a command similar to the following in the 
   product directory to get rid of the extra files (this assumes you have 
   not installed other non-JP Software files in the product directory 
   which this command would delete):

      *del /qe *.dat p*.dll *wise*.*

   If you are installing 4DOS under plain DOS (without Windows 9x/ME)
   you MUST use manual installation, as the automated installer is 
   Windows-based.
   
   When installing 4DOS under Windows 95/98 we recommend that you do NOT
   use a "long" directory name.  If you do, you will have to know and use
   the equivalent short name to load 4DOS in CONFIG.SYS; it's easier to
   simply use a short name from the beginning.  (This is not a 4DOS 
   limitation.  It's because DOS, which starts before Windows 95/98 and
   loads the primary command processor, cannot handle long file names.
   While 4DOS fully supports long file names, to make DOS work well it 
   is best to install 4DOS in a directory with a short name.)

   You can view any of the documentation files on-line, or copy them to
   your printer.  The Introduction and Installation Guide in 4DOS.TXT
   has page breaks, headers and footers; all other documentation files
   are unpaginated ASCII text.


FILES INCLUDED
--------------

   The following files are included with 4DOS 7.50:

         4DOS.COM          4DOS program file
         4HELP.EXE         4DOS help program
         4DOS.HLP          4DOS online help text
         4DOS.ICO          Icon file for 4DOS under Windows
         BATCOMP.EXE       Batch file compression utility
         CSCLEAN.BAT       Batch file used to restore CONFIG.SYS
                              during uninstall
         EXAMPLES.BTM      Sample batch file demonstrating the use of
	                     variable functions and internal variables
         HELPCFG.EXE       Color configuration program for the help
                              system
         INTRO.TXT         ASCII copy of the 4DOS Introduction and
                              Installation Guide
         KSTACK.COM        Memory-resident program used by the
                              KEYSTACK command
         LICENSE.TXT       License agreement
         OPTION.EXE        Program used by the OPTION command
         README.TXT        This file


TECHNICAL SUPPORT AND DOWNLOADS
-------------------------------

   This version of 4DOS is free and unsupported.  (Existing 4DOS customers
   are still eligible for support.)

   To download JP Software files, including trial versions of our Windows
   products (4NT and Take Command), visit:

      >> Our web site at http://jpsoft.com

      >> Our ftp site at ftp://jpsoft.com


LEGAL STUFF
-----------

Copyright 2004, JP Software Inc., All Rights Reserved.  Published by JP
Software Inc., P.O. Box 328, Chestertown, MD 21620, USA
phone 410-810-8818, fax 410-810-0026.

4DOS is a registered trademarks of JP Software Inc. JP Software, 
jpsoft.com, and all JP Software designs and logos are also trademarks 
of JP Software Inc. Other product and company names are trademarks of 
their respective owners.

