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
{ This type is defined for convenience and portability; it represents a code
  that indicates the type of a bookmark stored in a stream.  The value assigned
  to an instance of this type is used to ensure the correct instance of
  @link(AStreamBookmark) or one of its descendants is constructed to manage the
  bookmark when it is read from the stream.
}
type TStreamBookmarkType = longword;

const
  { This constant represents a type of stream bookmark and can be assigned to
    @link(AStreamBookmark.bookmarkType) by the initializer of an instance of
    @link(AStreamBookmark) or one of its descendants.  The value of this
    constant is used to ensure that the correct instance of
    @link(AStreamBookmark) or one of its descendants is constructed when
    reading bookmarks from a stream.

    The value of this constant indicates that the bookmark stored in the stream
    is a direct instance of @link(AStreamBookmark).
  }
  SBTYPE_BOOKMARK = 1;

  { This constant represents a type of stream bookmark and can be assigned to
    @link(AStreamBookmark.bookmarkType) by the initializer of an instance of
    @link(AStreamBookmark) or one of its descendants.  The value of this
    constant is used to ensure that the correct instance of
    @link(AStreamBookmark) or one of its descendants is constructed when
    reading bookmarks from a stream.

    The value of this constant indicates that the bookmark stored in the stream
    is a direct instance of @link(AStreamBookmarkSpan).
  }
  SBTYPE_SPAN = 2;

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
    { Indicates the type of bookmark represented }
    myBookmarkType: TStreamBookmarkType;

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
      then adds the number of bytes required to store the bookmark type and
      position reference.
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
    { Retrieve the type of bookmark represented by the instance.

      This value is used when reading bookmarks from a stream to ensure the
      correct instance of @link(AStreamBookmark) or its descendants is created.
    }
    function bookmarkType: TStreamBookmarkType; virtual;

    { Retrieve the position in the stream to which the bookmark refers. }
    function position: TStreamOffset; virtual;

    { Set the position in the stream to which the bookmark refers.

      @code(thisPosition) must be a positive value which specifies the offset,
      in bytes, from the beginning of the stream to which the bookmark refers.
      The beginning of the stream is always at offset zero (@code(0)).  If
      @code(thisPosition) is negative, this routine will do nothing.

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

// AStreamBookmarkSpan *********************************************************
{ This class represents a string bookmark that specifies a span: that is, a
  block of bytes within a stream.  The bookmark tracks not only the position
  of the span, where it begins, but also its size in bytes.

  To mark the beginning of a span, simply call
  @link(AStreamBookmark.setPosition AStreamBookmarkSpan.setPosition).
  To mark the end of a span, call
  @link(AStreamBookmarkSpan.markEndPosition) and the bookmark will automatically
  calculate the size of the span.
}
type AStreamBookmarkSpan = class(AStreamBookmark)
  protected
    { Stores the size of the span, in bytes }
    myLength: TStreamLength;

  public
    // Initializer
    function init: boolean; override;

    { Calculate the number of bytes required to stream the bookmark.

      This method builds upon the behavior inherited from
      @link(AStreamBookmark.selfStreamingLength): it calls the inherited
      routine, then adds the number of bytes required to store the length of the
      span.
    }
    function selfStreamingLength: TStreamIOSize; override;

    { Write the bookmark, and just the bookmark, to the given stream.

      This method builds upon the behavior inherited from
      @link(AStreamBookmark.writeSelfTo); it first calls the inherited routine,
      then writes the value of @link(AStreamBookmarkSpan.length).

      @return(The total number of bytes written to @code(Dest).)
    }
    function writeSelfTo(const Dest: AStream): TStreamIOSize; override;

    // CanStream implementation ------------------------------------------------
    { Read the bookmark from the given stream.

      This method builds upon the behavior inherited from
      @link(AStreamBookmark.readFrom); it first calls the inherited routine,
      then reads the value of @link(AStreamBookmarkSpan.length).

      @return(The total number of bytes read from @code(Source).)
    }
    function readFrom(const Source: AStream): TStreamIOSize; override;

    // CanPrint implementation -------------------------------------------------
    { Construct a string representation of the bookmark, suitable for output to
      a text-based device, such as a console.

      This method overrides the behavior inherited from
      @link(AStreamBookmark.toString).  It returns a string that is constructed
      based on the format specified by either
      @link(sbspStringRepresentationPlural) or
      @link(sbspStringRepresentationSingular), depending on the value of
      @link(AStreamBookmarkSpan.length Self.length).  In the default
      implementation of this method, this string will contain the name of the
      span as well as the positions in the stream where it begins and ends, and
      its length.
    }
    function toString: AnsiString; override;

    // Properties --------------------------------------------------------------
    { Retrieve the size of the span, in bytes. }
    function length: TStreamLength; virtual;

    { Retrieve the position in the stream at which the span comes to an end.

      This method calculates the value returned by adding the value of
      @link(AStreamBookmark.position Self.position) to the value of
      @link(AStreamBookmarkSpan.length Self.length).
    }
    function endPosition: TStreamOffset; virtual;

    { Set the position in the stream at which the span comes to an end.

      @code(thisPosition) must be a positive value which specifies the offset,
      in bytes, from the beginning of the stream where the span comes to an end.
      The beginning of the stream is always at offset zero (@code(0)).  If
      @code(thisPosition) is negative, this routine will do nothing.

      If @code(thisPosition) represents a position in the stream that is less
      than the value of @link(AStreamBookmark.position Self.position), this
      method will do nothing.

      @return(The previous value of @link(AStreamBookmarkSpan.endPosition).)
    }
    function setEndPosition(const thisPosition: TStreamOffset): TStreamOffset;
      virtual;

    { Use the current position in the specified stream as the point at which the
      span comes to an end.

      If @code(Source) is valid (not @nil), then this method calls
      @link(AStream.position) on it and stores the result.

      @return(The previous value of @link(AStreamBookmarkSpan.endPosition).)
    }
    function setEndToCurrentPositionIn(const Source: AStream): TStreamOffset;
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
      bookmark is not found, this method will return a negative value.
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

    { This method is responsible for constructing the appropriate instance of
      @link(AStreamBookmark) or one of its descendants to read the bookmark
      data stored in the stream.  Descendant classes may override this method
      to add to the types of bookmarks that are recognized; the base
      implementation will construct instances of @link(AStreamBookmark) or
      @link(AStreamBookmarkSpan).

      @return(An instance of @link(AStreamBookmark) or one of its descendants,
      depending on the value of @code(thisBookmarkType).  The caller is
      responsible for ensuring that the instance returned is freed when it is no
      longer needed.

      If @code(thisBookmarkType) specifies a value that is not recognized by the
      method, the method will return @nil.)
    }
    function NewBookmarkOfType(const thisBookmarkType: TStreamBookmarkType): AStreamBookmark;
      virtual;

    // CanStream implementation ------------------------------------------------
    { Read the dictionary from the given stream.

      This method overrides the behavior inherited from
      @link(ABinaryTree).  After reading the number of entries in the
      dictionary the method loops, reading the type of bookmark from the
      stream and then calling
      @link(AStreamBookmarkDictionary.NewBookmarkOfType).  Once the appropriate
      instance has been constructed, the @link(ANode.readFrom readFrom) method
      is called on it to ensure that the data is read. The bookmark instance
      is then added to the dictionary.

      @return(The total number of bytes read from the stream.  If
      @code(Source) is @nil, this routine will do nothing and will return
      zero (@code(0)).
    }
    function readFrom(const Source: AStream): TStreamIOSize; override;
end;
{$ENDIF}
{$IFDEF ReadResourceStrings}
  { The format specified by this string is used to construct a string
    representation of @link(AStreamBookmark) when
    @link(AStreamBookmark.toString) is called.

    The string placeholder is replaced by the name of the bookmark, as
    returned by a call to @link(AStringLeaf.name AStreamBookmark.name).

    The integer placeholder is replaced with the position in the stream to which
    the bookmark refers, as returned by a call to
    @link(AStreamBookmark.position).
  }
  stbmStringRepresentation = '%s => %d';

  { The format specified by this string is used to construct a string
    representation of @link(AStreamBookmarkSpan) when
    @link(AStreamBookmarkSpan.toString) is called.  This representation is used
    when @link(AStreamBookmarkSpan.length) returns any value except 1.

    The string placedholder is replaced by the name of the bookmark, as
    returned by a call to @link(AStringLeaf.name AStreamBookmark.name).

    The first integer placeholder is replaced with the position in the stream at
    which the span begins, as returned by a call to
    @link(AStreamBookmark.position AStreamBookmarkSpan.position).

    The second integer placeholder is replaced with the position in the stream
    at which the span ends, as returned by a call to
    @link(AStreamBookmarkSpan.endPosition).

    The third integer placeholder is replaced with the length of the span, in
    bytes, as returned by a call to
    @link(AStreamBookmarkSpan.length).
  }
  sbspStringRepresentationPlural = '%s => from %d to %d (%d bytes)';

  { The format specified by this string is used to construct a string
    representation of @link(AStreamBookmarkSpan) when
    @link(AStreamBookmarkSpan.toString) is called.  This representation is used
    when @link(AStreamBookmarkSpan.length) returns a value of 1.

    The string placedholder is replaced by the name of the bookmark, as
    returned by a call to @link(AStringLeaf.name AStreamBookmark.name).

    The first integer placeholder is replaced with the position in the stream at
    which the span begins, as returned by a call to
    @link(AStreamBookmark.position AStreamBookmarkSpan.position).

    The second integer placeholder is replaced with the position in the stream
    at which the span ends, as returned by a call to
    @link(AStreamBookmarkSpan.endPosition).

    The third integer placeholder is replaced with the length of the span, in
    bytes, as returned by a call to
    @link(AStreamBookmarkSpan.length).
  }
  sbspStringRepresentationSingular = '%s => from %d to %d (%d byte)';
{$ENDIF}
{$IFDEF ReadImplementation}
// AStreamBookmark *************************************************************
// Constructor
constructor AStreamBookmark.named(const thisKey: string;
  const thisPosition: TStreamOffset);

begin
  inherited named(thisKey);

  Self.setPosition(thisPosition);
end;

// Constructor
constructor AStreamBookmark.atCurrentPositionIn(const Source: AStream;
  const thisName: string);

begin
  inherited named(thisName);

  if Source <> nil then
    Self.setPosition(Source.position);
end;

// Initializer
function AStreamBookmark.init: boolean;

begin
  result := inherited init;

  if result then
  begin
    myBookmarkType := SBTYPE_BOOKMARK;
    myPosition := -1;
  end;
end;

// Calculate the number of bytes required to stream the bookmark
function AStreamBookmark.selfStreamingLength: TStreamIOSize;

begin
  // Call the inherited method first
  result := inherited selfStreamingLength;

  // Add ourselves
  inc(result, sizeof(myBookmarkType) + sizeof(myPosition));
end;

// Write the bookmark to the given stream
function AStreamBookmark.writeSelfTo(const Dest: AStream): TStreamIOSize;

begin
  result := 0;

  if Dest = nil then
    exit;

  // Write the bookmark type first
  result := Dest.write(sizeof(myBookmarkType), myBookmarkType);

  // Call the inherited routine
  result := result + inherited writeSelfTo(Dest);

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

  // Read the bookmark type first
  result := Source.read(sizeof(myBookmarkType), myBookmarkType);

  // Call the inherited routine
  result := result + inherited readFrom(Source);

  // Read the position to which the bookmark refers
  result := result + Source.read(sizeof(myPosition), myPosition);
end;

// CanPrint implementation -----------------------------------------------------
// Construct a string representation of the bookmark
function AStreamBookmark.toString: AnsiString;

begin
  result := SysUtils.format(stbmStringRepresentation, [
    Self.name, Self.position
  ]);
end;

// Properties ------------------------------------------------------------------
// Retrieve the bookmark type
function AStreamBookmark.bookmarkType: TStreamBookmarkType;

begin
  result := myBookmarkType;
end;

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
    Self.setPosition(Source.position);
end;

// AStreamBookmarkSpan *********************************************************
// Initializer
function AStreamBookmarkSpan.init: boolean;

begin
  result := inherited init;

  if result then
  begin
    myBookmarkType := SBTYPE_SPAN;
    myLength := 0;
  end;
end;

// Calculate the number of bytes required to stream the bookmark
function AStreamBookmarkSpan.selfStreamingLength: TStreamIOSize;

begin
  result := inherited selfStreamingLength + sizeof(myLength);
end;

// Write the bookmark to the given stream
function AStreamBookmarkSpan.writeSelfTo(const Dest: AStream): TStreamIOSize;

begin
  result := 0;

  if Dest <> nil then
  begin
    // Call the inherited routine first
    result := inherited writeSelfTo(Dest);
    // Write our length
    result := result + Dest.write(sizeof(myLength), myLength);
  end;
end;

// CanStream implementation ----------------------------------------------------
// Read the bookmark from the given stream
function AStreamBookmarkSpan.readFrom(const Source: AStream): TStreamIOSize;

begin
  result := 0;

  if Source <> nil then
  begin
    // Call the inherited routine first
    result := inherited readFrom(Source);
    // Read our length
    result := result + Source.read(sizeof(myLength), myLength);
  end;
end;

// CanPrint implementation -----------------------------------------------------
// Construct a string representation of the bookmark
function AStreamBookmarkSpan.toString: AnsiString;

begin
  if Self.length <> 1 then
    result := SysUtils.format(sbspStringRepresentationPlural, [
      Self.name, Self.position, Self.endPosition, Self.length
    ])

  else
    result := SysUtils.format(sbspStringRepresentationSingular, [
      Self.name, Self.position, Self.endPosition, Self.length
    ]);
end;

// Properties ------------------------------------------------------------------
// Retrieve the size of the span, in bytes
function AStreamBookmarkSpan.length: TStreamLength;

begin
  result := myLength;
end;

// Retrieve the position at which the span ends
function AStreamBookmarkSpan.endPosition: TStreamOffset;

begin
  result := Self.position + Self.length;
end;

// Mark the position at which the span ends
function AStreamBookmarkSpan.setEndPosition(const thisPosition: TStreamOffset): TStreamOffset;

begin
  // Return the previous end position
  result := Self.endPosition;

  if (thisPosition < 0) or (thisPosition < Self.position) then
    exit;

  // Calculate the length of the span
  myLength := thisPosition - Self.position;
end;

// Mark the position at which the span ends
function AStreamBookmarkSpan.setEndToCurrentPositionIn(const Source: AStream): TStreamOffset;

begin
  // Return the previous end position
  result := Self.endPosition;

  if (Source <> nil) and (Source.position > Self.position) then
    Self.setEndPosition(Source.position);
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
  result := -1;

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

// Read a bookmark of the given type from the given stream
function AStreamBookmarkDictionary.NewBookmarkOfType(const thisBookmarkType: TStreamBookmarkType): AStreamBookmark;

begin
  Result := nil;

  if thisBookmarkType = 0 then
    exit;

  case thisBookmarkType of
    SBTYPE_BOOKMARK:
      Result := AStreamBookmark.new;

    SBTYPE_SPAN:
      Result := AStreamBookmarkSpan.new;
  end;
end;

// CanStream implementation ----------------------------------------------------
// Read the bookmarks from the given stream
function AStreamBookmarkDictionary.readFrom(const Source: AStream): TStreamIOSize;

var
  // Stores the number of entries in the binary tree
  entryCount: longword;
  // Stores the type of bookmark stored in the stream
  thisBookmarkType: TStreamBookmarkType;
  // Refers to a new bookmark
  NewBookmark: AStreamBookmark;

begin
  result := 0;

  if Source = nil then
    exit;

  thisBookmarkType := 0;

  // Retrieve the entry count from the stream
  entryCount := 0;
  result := Source.read(sizeof(entryCount), entryCount);

  // Rebuild the dictionary
  while entryCount > 0 do
  begin
    // Read the bookmark type
    result := result + Source.read(sizeof(thisBookmarkType), thisBookmarkType);
    Source.rewindBy(sizeof(thisBookmarkType));

    // Construct the appropriate bookmark type
    NewBookmark := Self.NewBookmarkOfType(thisBookmarkType);

    // Add the bookmark
    if NewBookmark <> nil then
    begin
      // Read the bookmark
      result := result + NewBookmark.readFrom(Source);
      // Add the bookmark
      Self.addLeaf(NewBookmark)
    end

    // If we failed to read a bookmark, we can go no further
    else
      break;

    dec(entryCount);
  end;
end;
{$ENDIF}

