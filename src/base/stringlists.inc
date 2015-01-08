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
// AStringListItem *************************************************************
{ This class represents a single entry in @link(AStringList).  It extends
  @link(ANode) to allow each one to hold a string value.  The list item also
  supports streaming and sorting.
}
type AStringListItem = class(ANode)
  protected
    { Stores the value of the list item. }
    myText: string;

  public
    { Construct a new list item that has the specified value. }
    constructor withText(const thisText: string);

    { Initializer }
    function init: boolean; override;

    { Search the node, its children, and its older siblings for the specified
      value.

      This method performs a sequential search through each node and so can
      be inefficient for large lists.  It begins by recursively calling itself
      on any older siblings.  If the value is not found, then it searches the
      current node and its children.

      If this method is called on the last item in @link(AStringList), it will
      result in the entire list being searched.

      @return(A reference to the first node that contains the specified string
      value, if found; @nil if the value was not found.)
    }
    function find(const thisValue: string): AStringListItem; virtual;

    { Construct a shallow copy of the other object.

      This method overrides the behavior inherited from
      @link(AnObject.shallowCopyFrom): it calls that method, then checks to see
      whether @code(Other) is an instance of @link(AStringListItem).  If so,
      it copies the value of @link(AStringListItem.text) from @code(Other) to
      @code(Self), overwriting the value in @code(Self).

      Note that this method does NOT copy any sibling or child nodes and so
      cannot be used to create a full copy of any descendant of @link(ANode).
      The copy will NOT be automatically placed in the list to which
      @code(Other) belongs, if any, but the caller is free to do so.
    }
    function shallowCopyFrom(const Other: AnObject): boolean; override;

    { Calculate the number of bytes required to stream the node, and just the
      node.

      This method builds upon the behavior inherited from
      @link(ANode.selfStreamingLength): it calls the inherited routine, then
      calculates the number of bytes required to stream its text and adds that
      to the result, which is returned to the caller.
    }
    function selfStreamingLength: TStreamIOSize; override;

    { Write the node, and just the node, to the specified stream.  For more
      information on this method, see @link(ANode.writeSelfTo).

      This writes the value of its string by calling @link(AStream.writeString)
      on @code(Dest), then calls the inherited routine.

      @return(The total number of bytes written to @code(Dest).)
    }
    function writeSelfTo(const Dest: AStream): TStreamIOSize; override;

  // CanStream implementation --------------------------------------------------
    { Read the node from the specified stream.  For more information on this
      method, see @link(ANode.readFrom).

      This method reads the value of its string by calling
      @link(AStream.readString) on @code(Source), then calls the inherited
      routine.

      @return(The total number of bytes read from @code(Source).)
    }
    function readFrom(const Source: AStream): TStreamIOSize; override;

  // CanPrint implementation ---------------------------------------------------
    { Construct and return a string representation of the node.

      In accordance with the criteria described in
      @link(APrintingObject.toString), this method returns the value of
      @link(AStringListItem.text).
    }
    function toString: AnsiString; override;

  // Properties ----------------------------------------------------------------
    { Retrieve the string value associated with the node. }
    function text: string; virtual;

    { Set the string value associated with the node.

      @return(The old string value associated with the node.)
    }
    function setText(const newValue: string): string; virtual;
end;

// AStringList *****************************************************************
{ This class represents a list of one or more strings.  It extends
  @link(ALinkedList) to allow string values to be stored within the list.  As
  with @code(ALinkedList), @code(AStringList) may be accessed as though it was
  an indexed array or used like a stack.
}
type AStringList = class(ALinkedList)
  protected
    { Performs a quick sort }
    procedure quickSort(Left, Right: ANode); virtual;

  public
    { Construct a new string list that contains the specified strings.

      This method calls @link(AStringList.pushSeveral) to insert the strings in
      @code(strings) into the list after the new instance has been constructed
      and initialized.  If @code(strings) is empty, then an empty list is
      created by this routine.
    }
    constructor withStrings(const strings: array of string); virtual;

    { Construct a new string list by expanding the values provided in
      @code(thisList).

      If @code(thisDelimiter) is provided by the caller, then it is used to
      split the values provided in @code(thisList); otherwise, the delimiter
      specified by @link(slstDefaultDelimiter) is used.  See
      @link(AStringList.explode) for more information.

      If @code(thisList) is an empty string, then an empty list is created by
      this routine.
    }
    constructor from(const thisList: string;  const thisDelimiter: string = '');
      virtual;

    { Wrap the contents of a string that may span several lines so that it fits
      within the given line length.

      This method splits @code(str) into one or more tokens by splitting
      @code(str) wherever a character in @code(breakCharacters) occurs.  It then
      strings tokens together into one or more lines, ensuring that each line is
      no more than @code(lineLength) characters in length.  If @code(lineLength)
      is zero (0), then the value specified by @link(tosDefaultLineLength) is
      used.

      The first line is padded on the left by repeated @code(padCharacter)s
      unless @code(firstLineStartColumn) is zero (0).  Any subsequent lines are
      padded on the left by repeating @code(padCharacter)s only if
      @code(remainingLineStartColumn) is greater than zero (0).

      @code(breakCharacters) may specify UTF-8 or ASCII-encoded characters.
      Only the first UTF-8 or ASCII-encoded character in each string specified
      by @code(breakCharacters) is used to break @code(str); any remaining
      characters are ignored.  The default characters specified by
      @link(tosWrapDefaultBreakCharacters) may be used.

      @code(padCharacter) may specify a UTF-8 or ASCII-encoded character.  Only
      the first UTF-8 or ASCII-encoded character in @code(padCharacter) is used
      to pad each line; any remaining characters are ignored.  If
      @code(padCharacter) is an empty string, the value of
      @link(tosWrapDefaultPadCharacter) is used.

      @return(A string list that contains one item for each line of @code(str).
      The caller is responsible for freeing this list when it is no longer
      required.  If @code(str) is an empty string, this function will return
      @nil.)
    }
    class function wrapString(const str: string;
      const breakCharacters: array of string;
      lineLength: longword = tosDefaultLineLength;
      const firstLineStartColumn: longword = 0;
      const remainingLineStartColumn: longword = 0;
      padCharacter: string = ''): AStringList; virtual;

    { Initializer }
    function init: boolean; override;

    { Adds @code(thisString) to the end of the list.

      This method constructs a new instance of @link(AStringListItem) and then
      calls @link(ALinkedList.addItem) to add the item to the list.

      @return(The index of the new item.  This index is not guaranteed to
      remain the same for this item if other items are added or removed at
      locations PRIOR to the index returned.)
    }
    function add(const thisString: string): TNodeAbsoluteIndex; virtual;

    { Add several items to the end of the list.

      This method iterates through each item in @code(strings) and calls
      @link(AStringList.add) to add each item to the list.

      @return(The total number of items added to the list.)
    }
    function addSeveral(const strings: array of string): TCensus; virtual;

    { Add the items specified in @code(thisList) to the list.

      This method assumes that @code(thisList) contains one or more values that
      are separated by a delimiter of some kind (e.g., a comma).  It uses the
      value of @code(delimiter) to split the string into its component values;
      these values are then added as individual items to the list.  If
      @code(delimiter) is not supplied, or if it is an empty string, then
      the value of @link(llstDefaultDelimiter) is used.

      @return(The total number of items added to the list.)
    }
    function explode(thisList: string;
      delimiter: string = ''): TCensus; override;

    { Pushes the specified string value onto the list, as the last item in the
      list.  Calling @link(AStringList.pop) immediately after this routine will
      return @code(thisString).

      This method constructs a new instance of @link(AStringListItem) and then
      calls @link(ALinkedList.pushItem) to push the item onto the list.
    }
    procedure push(const thisString: string); virtual;

    { Push several items onto the list.

      This method iterates through each item in @code(strings) and calls
      @link(AStringList.push) to push each string value onto the list.
    }
    procedure pushSeveral(const strings: array of string); virtual;

    { Pops the last item from the list.  This will be the string most recently
      added to the list by a call to @link(AStringList.add) or
      @link(AStringList.push).

      If the list is empty, this routine returns an empty string.
    }
    function pop: string; virtual;

    { Inserts the specified string value at the given @code(index).  If an item
      already exists at that index, it is "bumped aside" in favor of the new
      item.

      This method constructs a new instance of @link(AStringListItem) and then
      calls @link(ALinkedList.insertItemAt) to insert the item into the list.

      The first item in the list is always at index zero (0).  The last item
      in the list is always at index (@link(ALinkedList.census) @code(-1)).
      If @code(index) specifies a value that is greater than the number of items
      in the list, then @code(thisString) is inserted at the end of the list.
    }
    procedure insertAt(const index: TNodeRelativeIndex;
      const thisString: string); virtual;

    { Performs a recursive search to determine whether or not the specified
      string exists within the list.  For very large lists, this search can
      be inefficient (and can cause stack overflows, due to the recursive
      nature of the search).

      This method calls @link(AStringListItem.find) on the last item in the
      list, which causes the entire list to be searched recursively.

      @return(@true if @code(thisString) is found in the list; @false if not.)
    }
    function hasText(const thisString: string): boolean; virtual;

    { Performs a linear search to find the index of the specified string.  The
      search will begin at @code(afterIndex).

      Because this method performs a linear search, it can be inefficient on
      very large lists.

      @return(The index of the specified string, if found; @code(-1) otherwise.
      The index returned by this routine will always be an offset relative to
      the beginning of the list (i.e., a positive integer) if the string is
      found.)
    }
    function indexOf(const thisString: string;
      const afterIndex: TNodeRelativeIndex = 0): TNodeRelativeIndex; virtual;

    { Sort the list based on the values of its items.

      This method uses a QuickSort to rearrange the items in the list.  If
      @code(reverse) is @true, then the items in the list are sorted from the
      highest value to the lowest value; otherwise, they are sorted from lowest
      value to highest value.

      In the default implementation, these items will be sorted alphabetically.
    }
    procedure sort(const reverse: boolean = false); virtual;

    // Properties --------------------------------------------------------------
    { Retrieve the value of the item at the specified index.

      This method calls @link(ALinkedList.itemAt) to retrieve the
      @link(AStringListItem) instance at the specified index; it then returns
      the value of @link(AStringListItem.text).

      Index zero (0) represents the first node in the list, while index
      (@link(ALinkedList.census) @code(- 1)) represents the last node in the
      list.  If @code(index) specifies a value that is greater than the number
      of nodes in the list, then this routine will remove the last item in
      the list.

      If an invalid index is passed to this routine, it will return an empty
      string.
    }
    function item(const thisIndex: TNodeRelativeIndex): string; virtual;

    { Set the value of the item at the specified index.

      This method calls @link(ALinkedList.itemAt) to retrieve the
      @link(AStringListItem) instance at the specified index; it then calls
      @link(AStringListItem.setText) on that instance.

      @return(The previous value of the string stored at the specified index,
      if any.  An empty string will be returned if an invalid index is passed
      to this routine.)
    }
    function setItem(const thisIndex: TNodeRelativeIndex;
      const thisValue: string): string;
end;
{$ENDIF}
{$IFDEF ReadImplementation}
// AStringListItem *************************************************************
{* Constructs and initializes new instance of a string list item.

   \return A new string list item instance on success; #nil on failure.
*}
constructor AStringListItem.withText(const thisText: string);

begin
  inherited new;
  myText := thisText;
end;

{* Initializes a new instance of a string list item.  This routine sets
   AStringListItem.value to an empty string and then calls the inherited
   initializer.

   \return #true if initialization was successful; #false if not.
*}
function AStringListItem.init: boolean;

begin
  myText := '';
  result := inherited init;
end;

// Find the specified value
function AStringListItem.find(const thisValue: string): AStringListItem;

begin
  Result := nil;

  // Check ourselves first
  if thisValue = myText then
    Result := Self

  else if Self.Previous <> nil then
    // Check older siblings
    Result := AStringListItem(Self.Previous).find(thisValue);
end;

// Construct a shallow copy of the given object
function AStringListItem.shallowCopyFrom(const Other: AnObject): boolean;

begin
  // Call the inherited routine first
  result := inherited shallowCopyFrom(Other);

  // Check to see whether the other item is compatible with us
  if not(Other is AStringListItem) then
    result := false;

  if result then
  begin
    myText := AStringListItem(Other).text;
  end;
end;

// Calculate the number of bytes required to stream the node
function AStringListItem.selfStreamingLength: TStreamIOSize;

begin
  // First call the inherited routine
  result := inherited selfStreamingLength;

  // Add the length of our text + length specifier + length checksum
  inc(result, System.length(myText) + (sizeof(TStreamIOSize) * 2));
end;

// Write the node, and just the node, to the specified stream
function AStringListItem.writeSelfTo(const Dest: AStream): TStreamIOSize;

begin
  result := 0;

  if Dest <> nil then
    result := Dest.writeString(myText);

  result := result + inherited writeSelfTo(Dest);
end;

// CanStream implementation ----------------------------------------------------
{* Reads the list item and its children from the specified stream.

   \return The total number of bytes read from \p Source.  This value will be
   zero (0) if \p Source is #nil.
*}
function AStringListItem.readFrom(const Source: AStream): TStreamIOSize;

begin
  result := 0;
  if Source <> nil then
    // Read our string value first
    result := Source.readString(myText);

  result := result + inherited readFrom(Source);
end;

// CanPrint implementation -----------------------------------------------------
{* Constructs a string representation of the item.  This routine checks the
   value of AStringListItem.value; if it is not an empty string, that value is
   returned.  Otherwise, this routine calls the inherited function
   (ANode.toString()).

   \return A string representation of the item, as described above.
*}
function AStringListItem.toString: AnsiString;

begin
  result := myText;
end;

// Properties ------------------------------------------------------------------
/// Retrieve the value of the item
function AStringListItem.text: string;

begin
  result := myText;
end;

/// Set the value of the item
function AStringListItem.setText(const newValue: string): string;

begin
  result := myText;
  myText := newValue;
end;

// AStringList *****************************************************************
// Construct a string list that contains the specified string values
constructor AStringList.withStrings(const strings: array of string);

begin
  inherited new;

  Self.pushSeveral(strings);
end;

{* Constructs and initializes a new instance of a string list.  This routine
   sets AStringList.delimiter to \p thisDelimiter and then calls
   AStringList.explode() to process \p thisList.

   \return A new instance of AStringList on success; #nil on failure.
*}
constructor AStringList.from(const thisList: string;
  const thisDelimiter: string = '');

begin
  inherited new;

  if thisList <> '' then
    Self.explode(thisList, thisDelimiter);
end;

// Split a string into one or more lines
class function AStringList.wrapString(const str: string;
  const breakCharacters: array of string;
  lineLength: longword = tosDefaultLineLength;
  const firstLineStartColumn: longword = 0;
  const remainingLineStartColumn: longword = 0;
  padCharacter: string = ''): AStringList;

var
  // Refers to the tokens parsed from the string
  Tokens: AStringList;
  // Loop counters
  i, j: TCensus;
  // Stores the number of characters in the string
  charLength: integer;
  // Stores a character from the string
  ch, nextChar: string;
  // Stores a token from the string
  token: string;
  // Stores a single line of text
  line: string;

begin
  Result := Self.new;

  if str = '' then
    exit;

  Tokens := Self.new;
  if Tokens = nil then
    exit;

  if lineLength = 0 then
    lineLength := tosDefaultLineLength;

  if padCharacter = '' then
    padCharacter := tosWrapDefaultPadCharacter;

  // Loop through the string, looking for tokens
  token := '';

  charLength := Charstring.census(str);

  for i := 1 to charLength do
  begin
    ch := Charstring.character(str, i);
    token := token + ch;

    if i < charLength then
      nextChar := Charstring.character(str, i + 1)

    else
      nextChar := '';

    // Check to see if we have a delimiter
    for j := 0 to (System.length(breakCharacters) - 1) do
    begin
      // Add the token to the broken string if a delimiter was found
      if ch = breakCharacters[j] then
      begin
        { Check that the next character is not a number first, so we don't break
          apart numbers.
        }
        if Charstring.positionOf(nextChar, charsDigits) = 0 then
        begin
          Tokens.push(token);
          token := '';
          break;
        end;
      end;
    end;
  end;

  // Push the last token
  Tokens.push(token);

  // Now that we have a string broken into tokens, assemble our lines
  line := '';

  if firstLineStartColumn > 0 then
    line := Charstring.ofCharacter(padCharacter, firstLineStartColumn);

  for i := 0 to (Tokens.census - 1) do
  begin
    // Break the line when it becomes too long
    if Charstring.census(line) +
      Charstring.census(Tokens.item(i)) >= lineLength then
    begin
      Result.push(line);
      line := '';

      if remainingLineStartColumn > 0 then
        line := Charstring.ofCharacter(padCharacter, remainingLineStartColumn);
    end;

    line := line + Tokens.item(i);
  end;

  // Push the last line
  //writeln('Line: ', line);
  Result.push(line);

  //writeln('Wrapped text:', lineEnding, #9, Result.implode(','));

  // Free the tokens
  Tokens.free;
end;

// Initializer
function AStringList.init: boolean;

begin
  result := inherited init;

  if result then
  begin
    MyItemType := AStringListItem;
  end;
end;

{* Adds the specified string value to the list.  This routine calls
   AList.addItem().

   \return The index of the new item.  This index is not guaranteed to be
   static, as a call to AStringList.insertAt() that specifies a lower index
   will push all higher indexes "down" the list.
*}
function AStringList.add(const thisString: string): TNodeAbsoluteIndex;

begin
  result := Self.addItem(AStringListItem.withText(thisString));
end;

// Add several strings to the list
function AStringList.addSeveral(const strings: array of string): TCensus;

var
  // Loop counter
  i: longword;

begin
  result := 0;

  if System.length(strings) = 0 then
    exit;

  for i := 0 to (System.high(strings)) do
  begin
    Self.add(strings[i]);
    inc(result);
  end;
end;

{* Builds the list from a delimited list of items.  This routine uses the value
   of delimiter to determine where to separate the list items.

   \return The number of items parsed from the list.  This value will be zero
   (0) if \p thisList is an empty string.
*}
function AStringList.explode(thisList: string;
  delimiter: string = ''): TCensus;

var
  // Stores an item retrieved from the list
  thisItem: string;

begin
  result := 0;

  if thisList <> '' then
  begin
    if delimiter = '' then
      delimiter := llstDefaultDelimiter;

    // Loop through the string
    while System.length(thisList) > 0 do
    begin
      thisItem := Charstring.chomp(thisList, delimiter);

      if thisItem <> '' then
      begin
        Self.add(thisItem);
        inc(result);
      end

      else
        break;
    end;
  end;
end;

{* Pushes the specified string onto the end of the list.  This routine calls
   AList.pushItem().
*}
procedure AStringList.push(const thisString: string);

begin
  Self.pushItem(AStringListItem.withText(thisString));
end;

// Push several strings onto the list
procedure AStringList.pushSeveral(const strings: array of string);

var
  // Loop counter
  i: longword;

begin
  if System.length(strings) = 0 then
    exit;

  for i := 0 to (System.high(strings)) do
    Self.push(strings[i]);
end;

{* Pops the last string value from the end of the list.  This routine calls
   AList.popItem().

   \return The last string from the list.  If there are no strings in the list,
   this routine will return an empty string.
*}
function AStringList.pop: string;

var
  // Refers to the item retrieved from the list
  ThisItem: AStringListItem;

begin
  result := '';
  ThisItem := AStringListItem(Self.PopItem);

  if ThisItem <> nil then
  begin
    result := ThisItem.text;
    ThisItem.free;
  end;
end;

{* Inserts a string at the specified index.  This routine calls
   AList.insertItemAt().
*}
procedure AStringList.insertAt(const index: TNodeRelativeIndex;
  const thisString: string);

begin
  Self.insertItemAt(index, AStringListItem.withText(thisString));
end;

// Determine whether the specified text is in the list
function AStringList.hasText(const thisString: string): boolean;

begin
  result := false;

  if MyLastItem <> nil then
    result := AStringListItem(MyLastItem).find(thisString) <> nil;
end;

// Find the index of the specified text
function AStringList.indexOf(const thisString: string;
  const afterIndex: TNodeRelativeIndex = 0): TNodeRelativeIndex;

var
  // Refers to the current item
  ThisItem: AStringListItem;
  // Stores the current index
  currentIndex: longint;

begin
  result := -1;

  ThisItem := AStringListItem(Self.itemAt(afterIndex));

  currentIndex := afterIndex;

  { Ensure we only use indices relative to the beginning of the list }
  if currentIndex < 0 then
    inc(currentIndex, myCensus + 1);

  if currentIndex < 0 then
    currentIndex := 0;

  while ThisItem <> nil do
  begin
    if ThisItem.text = thisString then
    begin
      result := currentIndex;
      break;
    end;

    ThisItem := AStringListItem(ThisItem.next);
    inc(currentIndex);
  end;
end;

procedure AStringList.quickSort(Left, Right: ANode);

var
  { Stores the node around which the list pivots }
  Pivot, Swap: ANode;
  { Stores the difference in "value" between two strings }
  diff: longint;

begin
  if Self.census <= 2 then
    exit;

  if Left = nil then
    Left := Self.FirstItem;

  if Right = nil then
    Right := Self.LastItem;

  if (Left = nil) or (Right = nil) then
    exit;

  Pivot := Right.Previous;

  if Pivot = nil then
    exit;

  while pointer(Left) <= pointer(Right) do
  begin
    diff := SysUtils.compareStr(AStringListItem(Left).text,
      AStringListItem(Pivot).text);

    writeln(SysUtils.format('Compared "%s" to pivot "%s"; result: %d', [
      AStringListItem(Left).text, AStringListItem(Pivot).text, diff
    ]));

    while diff < 0 do
    begin
      Left := Left.Next;
      if Left = nil then
        exit;

      diff := SysUtils.compareStr(AStringListItem(Left).text,
        AStringListItem(Pivot).text);
    writeln(SysUtils.format('Compared "%s" to pivot "%s"; result: %d', [
      AStringListItem(Left).text, AStringListItem(Pivot).text, diff
    ]));
    end;

    diff := SysUtils.compareStr(AStringListItem(Right).text,
      AStringListItem(Pivot).text);

    writeln(SysUtils.format('Compared "%s" to pivot "%s"; result: %d', [
      AStringListItem(Left).text, AStringListItem(Pivot).text, diff
    ]));

    while diff > 0 do
    begin
      Right := Right.Previous;
      if Right = nil then
        exit;

      diff := SysUtils.compareStr(AStringListItem(Right).text,
        AStringListItem(Pivot).text);
    writeln(SysUtils.format('Compared "%s" to pivot "%s"; result: %d', [
      AStringListItem(Left).text, AStringListItem(Pivot).text, diff
    ]));
    end;

    if pointer(Left) <= pointer(Right) then
    begin
      Swap := Left;
      Left := Right;
      Right := Swap;
      Left := Left.Next;
      Right := Right.Previous;
    end;

    Self.quicksort(Self.FirstItem, Right);
    Self.quicksort(Left, Self.LastItem);
  end;
end;

// Sort the list
procedure AStringList.sort(const reverse: boolean = false);

begin
  { A list of 1 or fewer items is already sorted }
  if myCensus < 2 then
    exit;

  Self.quickSort(MyFirstItem, MyLastItem);
end;

// Properties ------------------------------------------------------------------
{* Fetches the value of the string at the specified index.

   \return The value of the specified string.  If \p thisIndex is out of range,
   this routine will return an empty string.
*}
function AStringList.item(const thisIndex: TNodeRelativeIndex): string;

var
  // Refers to the item at the specified index
  ThisItem: AStringListItem;

begin
  result := '';
  ThisItem := AStringListItem(Self.ItemAt(thisIndex));

  if ThisItem <> nil then
    result := ThisItem.text;
end;

{* Sets the value of the string at the specified index.  If \p thisIndex is out
   of range, this routine will add a new item to the end of the list that
   contains the specified string.
*}
function AStringList.setItem(const thisIndex: TNodeRelativeIndex;
  const thisValue: string): string;

var
  // Refers to the item at the specified index
  ThisItem: AStringListItem;

begin
  result := '';

  // Fetch the item
  ThisItem := AStringListItem(Self.itemAt(thisIndex));
  // If the item was found, then set the value
  if ThisItem <> nil then
  begin
    result := ThisItem.text;
    ThisItem.setText(thisValue);
  end

  // Otherwise, insert the item at the end of the list
  else begin
    ThisItem := AStringListItem.withText(thisValue);
    if ThisItem <> nil then
      Self.pushItem(ThisItem);
  end;
end;
{$ENDIF}
