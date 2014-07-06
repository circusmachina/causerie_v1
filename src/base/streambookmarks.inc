{ streambookmarks.inc - A method for naming specific positions within a stream.

  This file is part of classwork.pp, a library of basic and useful classes.

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
// AStreamBookmark *************************************************************
{ This class represents a stream bookmark, which is a way of associating a
  name with a specific location in a stream.  This is useful in cases where
  you want to open an arbitrary stream and find the position of a specific
  piece of data, but you don't know ahead of time where that data will be in
  the stream.

  Stream bookmarks are managed by instances of @link(AStreamBookmarkDictionary),
  which provides a way to look up a specific bookmark and which can also
  read and write a list of bookmarks to a given stream.
}
type AStreamBookmark = class(AStringLeaf)
  protected
    { Stores the position in the stream where the data referenced by the
      bookmark may be found.
    }
    myPosition: TStreamOffset;

  public
    { Construct a new bookmark with the given name and given stream position. }
    constructor named(const thisKey: string;
      const thisPosition: TStreamOffset); reintroduce; virtual;

    { Construct a new bookmark with the given name, using the current position
      within the specified stream.

      If @code(Source) is valid (not @nil), this method will call
      @link(AStream.position) on it and use the value returned as the position
      to which the bookmark refers.
    }
    constructor atCurrentPositionIn(const Source: AStream;
      const thisName: string); virtual;

    { Initializer }
    function init: boolean; override;

    { Calculate the number of bytes required to stream the bookmark.

      This method builds upon the behavior inherited from
      @link(AStringLeaf.selfStreamingLength): it calls the inherited routine,
      then adds the number of bytes required to store the position reference.
    }
    function selfStreamingLength: TStreamIOSize; override;

    { Write the bookmark, and just the bookmark, to the given stream.

      This method builds upon the behavior inherited from
      @link(AStringLeaf.writeSelfTo); it first calls the inherited routine,
      then writes the value of @link(AStreamBookmark.position).

      @return(The total number of bytes written to @code(Dest).)
    }
    function writeSelfTo(const Dest: AStream): TStreamIOSize; override;

    // CanStream implementation ------------------------------------------------
    { Read the bookmark from the given stream.

      This method builds upon the behavior inherited from
      @link(AStringLeaf.readFrom); it first calls the inherited routine,
      then reads the value of @link(AStreamBookmark.position).

      @return(The total number of bytes read from @code(Source).)
    }
    function readFrom(const Source: AStream): TStreamIOSize; override;

    // CanPrint implementation -------------------------------------------------
    { Construct a string representation of the bookmark, suitable for output to
      a text-based device, such as a console.

      This method overrides the behavior inherited from
      @link(AStringLeaf.toString).  It returns a string that is constructed
      based on the format specified by @link(stbmStringRepresentation).  In
      the default implementation, this string will contain the name of the
      bookmark as well as the position in the stream to which it refers.
    }
    function toString: AnsiString; override;

    // Properties --------------------------------------------------------------
    { Retrieve the position in the stream to which the bookmark refers. }
    function position: TStreamOffset; virtual;

    { Set the position in the stream to which the bookmark refers.

      @return(The previous value of @link(AStreamBookmark.position).)
    }
    function setPosition(const newPosition: TStreamOffset): TStreamOffset;
      virtual;

    { Set the position in the stream to which the bookmark refers by
      querying the specified stream.

      If @code(Source) is valid (not @nil), then this method calls
      @link(AStream.position) on it and stores the result.

      @return(The previous value of @link(AStreamBookmark.position).)
    }
    function setToCurrentPositionIn(const Source: AStream): TStreamOffset;
      virtual;
end;

// AStreamBookmarkDictionary ***************************************************
{ This class represents a dictionary of stream bookmarks, which is a collection
  of instances of @link(AStreamBookmark).  It provides a way to retrieve the
  position to which a given bookmark refers and can read and write an entire
  list of bookmarks using an arbitrary stream.
}
type AStreamBookmarkDictionary = class(AStringTree)
  public
    { Initializer }
    function init: boolean; override;

    { Retrieve the position to which the named bookmark refers.

      If @code(thisBookmarkName) is invalid (an empty string), or the named
      bookmark is not found, this method will return a very large negative
      number (@link(STREAM_OFFSET_MIN)).  This is because stream bookmarks can
      refer to positions relative to the end of the stream (-1 or lower) OR to
      positions relative to the beginning of the stream (0 or greater).  It is
      unlikely that a stream bookmark will refer to a position relative to the
      end of the stream which is as large as the value returned by this method
      in the event of an error, however.
    }
    function positionOf(const thisBookmarkName: string): TStreamOffset; virtual;

    { Retrieve a reference to the named bookmark.  The reference returned by
      this routine should NOT be freed by the caller.

      This method simply calls @link(ABinaryTree.FetchLeaf) and casts the
      value returned to @link(AStreamBookmark).

      If the named bookmark is not found, this method returns @nil.
    }
    function BookmarkNamed(const thisBookmarkName: string): AStreamBookmark;
      virtual;
end;

const
  { This constant represents the largest possible negative stream offset.
    Negative stream offsets are used to refer to positions within instances of
    @link(AStream) relative to the end of the stream; in such cases, the end of
    the stream is always at position -1.  However, it is unlikely that a
    negative offset as large as this one will ever be used, which is why
    @link(AStreamBookmarkDictionary.positionOf) returns this value to indicate
    an error has occurred.
  }
  STREAM_OFFSET_MIN: int64 = -9223372036854775808;
{$ENDIF}
{$IFDEF ReadResourceStrings}
  { The format specified by this string is used to construct a string
    representation of @link(AStreamBookmark) when
    @link(AStreamBookmark.toString) is called.

    The string placeholder is replaced by the name of the bookmark, as
    returned by a call to @link(AStringLeaf.name AStreamBookmark.name).  The
    integer placeholder is replaced with the position in the stream to which
    the bookmark refers, as returned by a call to
    @link(AStreamBookmark.position).
  }
  stbmStringRepresentation = '%s => %d';
{$ENDIF}
{$IFDEF ReadImplementation}
// AStreamBookmark *************************************************************
// Constructor
constructor AStreamBookmark.named(const thisKey: string;
  const thisPosition: TStreamOffset);

begin
  inherited named(thisKey);

  myPosition := thisPosition;
end;

// Constructor
constructor AStreamBookmark.atCurrentPositionIn(const Source: AStream;
  const thisName: string);

begin
  inherited named(thisName);

  if Source <> nil then
    myPosition := Source.position;
end;

// Initializer
function AStreamBookmark.init: boolean;

begin
  result := inherited init;

  if result then
  begin
    myPosition := 0;
  end;
end;

// Calculate the number of bytes required to stream the bookmark
function AStreamBookmark.selfStreamingLength: TStreamIOSize;

begin
  // Call the inherited method first
  result := inherited selfStreamingLength;

  // Add ourselves
  inc(result, sizeof(myPosition));
end;

// Write the bookmark to the given stream
function AStreamBookmark.writeSelfTo(const Dest: AStream): TStreamIOSize;

begin
  result := 0;

  if Dest = nil then
    exit;

  // Call the inherited routine first
  result := inherited writeSelfTo(Dest);

  // Write the position to which the bookmark refers
  result := result + Dest.write(sizeof(myPosition), myPosition);
end;

// CanStream implementation ----------------------------------------------------
// Read the bookmark from the given stream
function AStreamBookmark.readFrom(const Source: AStream): TStreamIOSize;

begin
  result := 0;

  if Source = nil then
    exit;

  // Call the inherited routine first
  result := inherited readFrom(Source);

  // Read the position to which the bookmark refers
  result := result + Source.read(sizeof(myPosition), myPosition);
end;

// CanPrint implementation -----------------------------------------------------
// Construct a string representation of the bookmark
function AStreamBookmark.toString: AnsiString;

begin
  result := SysUtils.format(stbmStringRepresentation, [
    Self.name, myPosition
  ]);
end;

// Properties ------------------------------------------------------------------
// Retrieve the position to which the bookmark refers
function AStreamBookmark.position: TStreamOffset;

begin
  result := myPosition;
end;

// Set the position to which the bookmark refers
function AStreamBookmark.setPosition(const newPosition: TStreamOffset): TStreamOffset;

begin
  // Return the old position
  result := myPosition;

  myPosition := newPosition;
end;

// Set the position to which the bookmark refers
function AStreamBookmark.setToCurrentPositionIn(const Source: AStream): TStreamOffset;

begin
  // Return the old position
  result := myPosition;

  if Source <> nil then
    myPosition := Source.position;
end;

// AStreamBookmarkDictionary ***************************************************
// Initializer
function AStreamBookmarkDictionary.init: boolean;

begin
  result := inherited init;

  if result then
  begin
    // By default, we manage instances of AStreamBookmark
    MyLeafType := AStreamBookmark;
  end;
end;

// Retrieve the position referred to by the named bookmark
function AStreamBookmarkDictionary.positionOf(const thisBookmarkName: string): TStreamOffset;

var
  // Refers to the named bookmark
  ThisBookmark: AStreamBookmark;

begin
  result := STREAM_OFFSET_MIN;

  if thisBookmarkName = '' then
    exit;

  ThisBookmark := Self.BookmarkNamed(thisBookmarkName);

  if ThisBookmark <> nil then
    result := ThisBookmark.position;
end;

// Retrieve a reference to the named bookmark
function AStreamBookmarkDictionary.BookmarkNamed(const thisBookmarkName: string): AStreamBookmark;

begin
  Result := AStreamBookmark(Self.FetchLeaf(thisBookmarkName));
end;
{$ENDIF}

