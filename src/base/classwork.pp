{ classwork.pp - A library of basic Pascal classes useful in larger projects.
  Copyright (C) 2011-2014 Michael Malicoat.

  This program is free software: you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 3 of the License or, at your
  option, any later version.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABLITY
  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
  License for more details.

  You should have received a copy of the GNU General Public License along
  with this program.  If not, see <http://www.gnu.org/licenses/>.
}

{$SMARTLINK ON}

{ This unit defines several classes to handle:
    @unorderedlist(
      @item(basic object types: @link(AnObject))
      @item(streaming I/O: @link(AStream) and @link(AFileStream) and their
            descendants)
      @item(exceptions: @link(AnException) and its descendants)
      @item(linked lists: @link(ANode), @link(ALinkedList) and their
            descendants)
      @item(binary trees: @link(ABinaryLeaf), @link(ABinaryTree) and their
            descendants)
      @item(logging: @link(ALoggedItem), @link(ALog) and their descendants)
      @item(string handling: @link(Charstring))
      @item(iteration: @link(CanIterate) and @link(AnIterator) and its
            descendants)
      @item(vectors: @link(AVector) and its descendants)
      @item(basic application support: @link(AnApplication) and its descendants)
  )

  All of this functionality routinely serves as the basis for my projects, and
  so I have now condensed what were several units into one for increased
  portability between projects.
}
unit classwork;

{$mode objfpc}{$H+}

interface
// We are not using globally-unique interface IDs
{$INTERFACES CORBA}

{$DEFINE ReadInterface}
{$I classwork.inc}
{$I iterators.inc}
{$I streams.inc}
{$I streamiterators.inc}
{$I streamwrappers.inc}
{$I textstreams.inc}
{$I strings.inc}
{$I printingobjects.inc}
{$I exceptions.inc}
{$I bufferedstreams.inc}
{$I filestreams.inc}
{$I consolestreams.inc}
{$I stringstreams.inc}
{$I nodes.inc}
{$I vectors.inc}
{$I pointervectors.inc}
{$I objectvectors.inc}
{$I binarytrees.inc}
{$I binarytreeiterators.inc}
{$I stringtrees.inc}
{$I linkedlists.inc}
{$I stringlists.inc}
{$I variantlists.inc}
{$I mappings.inc}
{$I dictionaries.inc}
{$I streambookmarks.inc}
{$I bookmarkedstreams.inc}
{$I counters.inc}
{$I loggeditems.inc}
{$I logging.inc}
{$I loggingobjects.inc}
{$I memory.inc}
{$I versioninfo.inc}
{$I applications.inc}
{$I consoleapplications.inc}

{$UNDEF ReadInterface}

resourcestring
  {$DEFINE ReadResourceStrings}
  {$I classwork.inc}
  {$I streams.inc}
  {$I exceptions.inc}
  {$I textstreams.inc}
  {$I strings.inc}
  {$I filestreams.inc}
  {$I bufferedstreams.inc}
  {$I nodes.inc}
  {$I binarytrees.inc}
  {$I stringtrees.inc}
  {$I linkedlists.inc}
  {$I mappings.inc}
  {$I streambookmarks.inc}
  {$I counters.inc}
  {$I loggeditems.inc}
  {$I vectors.inc}
  {$I versioninfo.inc}

  {$UNDEF ReadResourceStrings}

implementation

uses
{$IFDEF WINDOWS}
  windows,
{$ENDIF}
  crc, lazutf8, sysUtils, variants, video;

{$DEFINE ReadImplementation}
{$I classwork.inc}
{$I streams.inc}
{$I streamiterators.inc}
{$I streamwrappers.inc}
{$I textstreams.inc}
{$I strings.inc}
{$I printingobjects.inc}
{$I exceptions.inc}
{$I bufferedstreams.inc}
{$I filestreams.inc}
{$I consolestreams.inc}
{$I stringstreams.inc}
{$I nodes.inc}
{$I vectors.inc}
{$I pointervectors.inc}
{$I objectvectors.inc}
{$I binarytrees.inc}
{$I binarytreeiterators.inc}
{$I stringtrees.inc}
{$I linkedlists.inc}
{$I stringlists.inc}
{$I variantlists.inc}
{$I mappings.inc}
{$I dictionaries.inc}
{$I streambookmarks.inc}
{$I bookmarkedstreams.inc}
{$I counters.inc}
{$I loggeditems.inc}
{$I logging.inc}
{$I loggingobjects.inc}
{$I memory.inc}
{$I versioninfo.inc}
{$I applications.inc}
{$I consoleapplications.inc}

{$UNDEF ReadImplementation}

end.

