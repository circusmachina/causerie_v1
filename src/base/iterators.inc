{ This file is part of classwork.pp, a library of basic and useful classes.

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
{$IFDEF ReadInterface}
{ This class represents an abstract iterator.

  This class is not meant to be instantiated directly; rather, it defines those
  methods which its descendants are required to implement.  In addition to
  the methods defined by this class, descendants will also provide some method
  for retrieving and manipulating the "current" item.

  Note that when an instance of @link(AnIterator) or its descendants is created,
  they are expected to position themselves at the first item to be returned;
  this behavior allows calls to @link(AnIterator.next) to be positioned at the
  bottom of a loop, as shown in the following example:  @longcode(#

  // Calculate a checksum for the stream
  function AStream.checksum: int64;

  var
    // Stores the size of the file
    streamSize: int64;
    // Stores the position in the stream before the checksum was calculated
    previousPosition: int64;
    // Used to iterate over the stream
    Element: AStreamIterator;

  begin
    result := 0;
    streamSize := Self.length;
    if streamSize > 0 then
    begin
      // Store the current position
      previousPosition := Self.position;

      Element := AStreamIterator(Self.iterator);

      while Element.continues do
      begin
        result := result + Element.value;
        Element.next;
      end;

      Element.free;
      // Seek the previous position
      Self.setPosition(previousPosition);
    end;
  end;
  #)
}
type AnIterator = class(AnObject)
  public
    { Increments the iterator.

      This method is expected to fail silently if there are no further items
      to retrieve from the source.
    }
    procedure next; virtual; abstract;

    { Determines whether are not there are more items ready for iteration.

      This method should return @false when the end of the source has been
      reached, so that the calling loop can end correctly.
    }
    function continues: boolean; virtual; abstract;
end;

{ This iterface declares the methods that must be implemented by any class
  that supports iteration over its data.
}
type CanIterate = interface
  { Constructs and initializes an iterator that is suitable for use with the
    object and its data.  The iterator returned by this function can be used
    to iterate over the data maintained by the object.

    @return(A new instance of one of @link(AnIterator)'s descendants that is
    suitable for iterating over the object's data.  It is the responsibility of
    the caller to free this iterator when it is no longer needed.)
  }
  function iterator: AnIterator;
end;
{$ENDIF}


