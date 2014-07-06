{ bookmarkedstreams.inc - Streams that can bookmark themselves.

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
{ This class represents a bookmarked stream, which is a stream that is capable
  of saving and reading its own list of bookmarks.

  Instances of @link(ABookmarkedStream) do not themselves record the position
  of their bookmark dictionary; this allows descendant classes to specify
  where that value is recorded.

  You will probably not create an instance of @link(ABookmarkedStream) directly
  as it provides no functionality for reading or writing; instead, you will use
  either @link(ABookmarkedInputStream) or @link(ABookmarkedOutputStream).
}
type ABookmarkedStream = class(AStreamWrapper)
  protected
    { Refers to the dictionary of bookmarks maintained for the stream }
    MyBookmarks: AStreamBookmarkDictionary;

  public
    { Construct a new bookmarked stream instance that will wrap around the
      given stream.

      If @code(takeOwnershipOfTarget) is @true, then the bookmarked stream
      instance will assume ownership of @code(Target) and will free it when it
      is, itself freed.
    }
    constructor around(const ThisTarget: AStream;
      const takeOwnershipOfTarget: boolean = true); virtual;

    { Initializer }
    function init: boolean; override;

    { Free the bookmarked stream.

      If @link(AStreamWrapper.ownsTarget ABookmarkedStream.ownsTarget) is
      @true, then this method will also free
      @code(AStreamWrapper.Target ABookmarkedStream.Target).
    }
    destructor destroy; override;

    { Set the current position in the stream to the one specified by the
      named bookmark.

      If no bookmark with @code(thisBookmarkName) is found in the dictionary of
      bookmarks maintained by the stream, then this routine does nothing.

      @return(The previous position within the stream.  This will always be
      an offset from the beginning of the stream.)
    }
    function seek(const thisBookmarkName: string): TStreamOffset;
      virtual;

    // Properties --------------------------------------------------------------
    { Retrieve a reference to the dictionary of bookmarks maintained by the
      stream.

      The reference returned by this routine may be used to query the dictionary
      of bookmarks, but it should NOT be freed by the caller.
    }
    function Bookmarks: AStreamBookmarkDictionary; virtual;
end;

// ABookmarkedOutputStream *****************************************************
{ This class represents a stream that is capable of recording bookmarks to a
  dictionary of bookmarks and then writing that dictionary to itself at a given
  position.

  Instances of @link(ABookmarkedOutputStream) are not able to read their
  dictionary of bookmarks, but they can write it.  The idea is that, as the
  stream is written, various bookmarks are recorded which are then also written
  to the stream before it is closed.
}
type ABookmarkedOutputStream = class(ABookmarkedStream)
  public
    { Write @code(count) bytes from @code(source) to the stream.

      This method overrides the behavior inherited from
      @link(AStreamWrapper) (which does nothing); it passes the call through
      to @link(AStreamWrapper.Target ABookmarkedOutputStream.Target) and
      returns the result.

      @return(The total number of bytes written to the stream.)
    }
    function write(const count: TStreamIOSize; const source): TStreamIOSize;
      override;

    { Record the current stream position using a bookmark with the given name.

      If a bookmark with @code(thisBookmarkName) already exists in the
      dictionary of bookmarks maintained by the stream, then the value of that
      bookmark is updated.  Otherwise, a new bookmark is created and entered
      into the dictionary.

      If @code(thisBookmarkName) is an empty string, this routine does nothing.

      @return(A reference to the bookmark that was updated.  This will either
      be a new bookmark or an existing bookmark that had
      @code(thisBookmarkName).  In either case, the caller should NOT free the
      reference returned by this routine; that will be done when the bookmarked
      stream is destroyed.)
    }
    function MarkPosition(const thisBookmarkName: string): AStreamBookmark;
      virtual;

    { Write the dictionary of bookmarks to the stream at the specified
      position.

      This method moves the stream position pointer to the position specified
      and then calls
      @link(ANode.writeTo AStreamBookmarkDictionary.writeTo) on
      @link(ABookmarkedStream.Bookmarks ABookmarkedOutputStream.Bookmarks),
      passing @code(Self) as the destination stream.  By default, the
      dictionary of bookmarks is written at the end of the stream, but
      the caller may wish to store the dictionary at another position in the
      stream.  If so, it may be helpful to first call
      @link(ANode.streamingLength AStreamBookmarkDictionary.streamingLength)
      to determine how much space in the stream will be required by the
      dictionary and ensure there is enough empty space in the stream

      @return(The position in the stream (relative to the beginning of the
      stream) at which the bookmarks were written.  This value can be used,
      among other things, as part of a stream header so that the bookmarks can
      be recalled from the stream later.  If the bookmarks could not be
      written, this routine will return a negative value.)
    }
    function writeBookmarksAt(const thisPosition: TStreamOffset = -1): TStreamOffset;
      virtual;
end;

// ABookmarkedInputStream ******************************************************
{ This class represents a stream that is capable of reading a dictionary of
  bookmarks from itself.

  Instances of @link(ABookmarkedInputStream) are not able to write their
  dictionary of bookmarks, but they can read it.  The idea is that, as soon as
  the stream is created or opened, the dictionary of bookmarks is read so that
  it can then be used to find specific data.
}
type ABookmarkedInputStream = class(ABookmarkedStream)
  public
    { Read @code(count) bytes from the stream and place them into @code(dest).

      This method overrides the behavior inherited from
      @link(AStreamWrapper) (which does nothing); it passes the call to
      @link(AStreamWrapper.Target ABookmarkedInputStream.Target) and returns
      the result.

      @returns(The total number of bytes read from the stream.)
    }
    function read(const count: TStreamIOSize; var dest): TStreamIOSize;
      override;

    { Read the dictionary of bookmarks from the specified position within the
      stream.

      This method moves the stream position pointer to the position specified
      and then calls
      @link(ABinaryTree.readFrom AStreamBookmarkDictionary.readFrom) on
      @link(ABookmarkedStream.Bookmarks ABookmarkedInputStream.Bookmarks),
      passing @code(Self) as the source stream.  The stream position pointer
      is then returned to its previous location.

      @return(The total number of bytes read from the stream.)
    }
    function readBookmarksAt(const thisPosition: TStreamOffset): TStreamIOSize;
      virtual;
end;
{$ENDIF}
{$IFDEF ReadImplementation}
// ABookmarkedStream ***********************************************************
// Constructor
constructor ABookmarkedStream.around(const ThisTarget: AStream;
  const takeOwnershipOfTarget: boolean = true);

begin
  inherited new;

  MyTarget := ThisTarget;
  myOwnTarget := takeOwnershipOfTarget;
end;

// Initializer
function ABookmarkedStream.init: boolean;

begin
  result := inherited init;

  if result then
  begin
    MyBookmarks := AStreamBookmarkDictionary.new;

    result := MyBookmarks <> nil;
  end;
end;

// Destructor
destructor ABookmarkedStream.destroy;

begin
  // Free the dictionary of bookmarks
  if MyBookmarks <> nil then
    MyBookmarks.free;

  // Call the inherited routine
  inherited destroy;
end;

// Seek the position specified by the named bookmark
function ABookmarkedStream.seek(const thisBookmarkName: string): TStreamOffset;

var
  // Refers to the named bookmark
  ThisBookmark: AStreamBookmark;

begin
  // Return the previous position in the stream
  result := Self.position;

  // Fetch the named bookmark
  ThisBookmark := MyBookmarks.BookmarkNamed(thisBookmarkName);

  if ThisBookmark <> nil then
    // Move to the specified position
    Self.setPosition(ThisBookmark.position);
end;

// Properties ------------------------------------------------------------------
// Retrieve a reference to the dictionary of bookmarks
function ABookmarkedStream.Bookmarks: AStreamBookmarkDictionary;

begin
  Result := MyBookmarks;
end;

// ABookmarkedOutputStream *****************************************************
// Write to the stream
function ABookmarkedOutputStream.write(const count: TStreamIOSize;
  const source): TStreamIOSize;

begin
  result := 0;

  // Pass the call to the target
  if MyTarget <> nil then
    result := MyTarget.write(count, source);
end;

// Mark the current position in the stream
function ABookmarkedOutputStream.MarkPosition(const thisBookmarkName: string): AStreamBookmark;

begin
  Result := nil;

  if (thisBookmarkName = '') or (MyTarget = nil) then
    exit;

  // Check to see whether the bookmark already exists
  Result := MyBookmarks.BookmarkNamed(thisBookmarkName);

  // Not an existing bookmark, so create a new one
  if Result = nil then
  begin
    Result := AStreamBookmark.atCurrentPositionIn(Self, thisBookmarkName);
    // Insert it into the dictionary
    MyBookmarks.insertLeaf(Result);
  end

  else
    // Update the position of an existing bookmark
    Result.setPosition(Self.position);
end;

// Write the dictionary of bookmarks to the specified position in the stream
function ABookmarkedOutputStream.writeBookmarksAt(const thisPosition: TStreamOffset = -1): TStreamOffset;

var
  // Stores the previous position within the stream
  previousPosition: TStreamOffset;

begin
  // Get the current position in the stream
  previousPosition := Self.position;

  // Move to the specified position
  Self.setPosition(thisPosition);
  // and store the current absolute position for return to the caller
  result := Self.position;

  // Write the bookmarks
  if MyBookmarks.writeTo(Self) = 0 then
    result := -1;

  // Return to the previous position in the stream
  Self.setPosition(previousPosition);
end;

// ABookmarkedInputStream ******************************************************
// Read from the stream
function ABookmarkedInputStream.read(const count: TStreamIOSize;
  var dest): TStreamIOSize;

begin
  result := 0;

  // Pass the call through to the target
  if MyTarget <> nil then
    result := MyTarget.read(count, dest);
end;

// Read the list of bookmarks from the stream
function ABookmarkedInputStream.readBookmarksAt(const thisPosition: TStreamOffset): TStreamIOSize;

var
  // Stores the previous position within the stream
  previousPosition: TStreamOffset;

begin
  // Get the current position in the stream
  previousPosition := Self.position;

  // Move to the specified position
  Self.setPosition(thisPosition);

  // Read the bookmarks
  result := MyBookmarks.readFrom(Self);

  // Return to the previous position in the stream
  Self.setPosition(previousPosition);
end;

{$ENDIF}

