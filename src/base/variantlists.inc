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
// AVariantListItem ************************************************************
{ This class represents a single item in a linked-list of items.  Each node
  represents a single @code(Variant) value which, when coupled with the
  FreePascal runtime, can hold different types of values.
}
type AVariantListItem = class(ANode)
  protected
    { Stores the value of the item }
    myValue: Variant;

  public
    { Construct a new list item that contains the specified value. }
    constructor withValue(const thisValue: Variant);

    { Initializer }
    function init: boolean; override;

    { Search the node, its children, and its older siblings for the specified
      value.

      This method performs a sequential search through each node and so can
      be inefficient for large lists.  It begins by recursively calling itself
      on any older siblings.  If the value is not found, then it searches the
      current node and its children.

      If this method is called on the last item in @link(AVariantList), it will
      result in the entire list being searched.

      @return(A reference to the first node that contains the specified variant
      value, if found; @nil if the value was not found.)
    }
    function find(const thisValue: Variant): AVariantListItem;

    { Construct a shallow copy of the other object.

      This method overrides the behavior inherited from
      @link(AnObject.shallowCopyFrom): it calls that method, then checks to
      see whether @code(Other) is an instance of @link(AVariantListItem).  If
      so, it copies the value of @link(AVariantListItem.value) from @code(Other)
      to @code(Self), overwriting the value in @code(Self).

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
      adds the number of bytes required to store an instance of @code(TVarData).
    }
    function selfStreamingLength: TStreamIOSize; override;

    { Write the node, and just the node, to the specified stream.  For more
      information on this method, see @link(ANode.writeSelfTo).

      This method casts @link(AVariantListItem.value) to
      @code(TVarData) and writes the resulting record to @code(Dest).

      @return(The total number of bytes written to @code(Dest).)
    }
    function writeSelfTo(const Dest: AStream): TStreamIOSize; override;

  // CanStream implementation --------------------------------------------------
    { Read the node from the specified stream.  For more information on this
      method, see @link(ANode.readFrom).

      This method reads @link(AVariantListItem.value) by reading a single
      record of type @code(TVarData) from @code(Source).  It then calls the
      inherited routine.

      @return(The total number of bytes read from @code(Source).)
    }
    function readFrom(const Source: AStream): TStreamIOSize; override;

  // CanPrint implementation ---------------------------------------------------
    { Construct and return a string representation of the node.

      In accordance with the criteria described in
      @link(APrintingObject.toString), this method calls
      @code(Variants.varToStr) to cast @link(AVariantListItem.value Self.value)
      to a string, then returns the resulting value.
    }
    function toString: AnsiString; override;

  // Properties ----------------------------------------------------------------
    { Retrieve the value associated with the item. }
    function value: Variant; virtual;

    { Set the value associated with the node.

      @return(The previous value of the item.)
    }
    function setValue(const newValue: Variant): Variant; virtual;
end;

// AVariantList ****************************************************************
{ This class represents a list of one or more variant values.  It extends
  @link(ALinkedList) to allow variant values to be stored within the list.  As
  with @code(ALinkedList), @code(AVariantList) may be accessed as though it was
  an indexed array or used like a stack.
}
type AVariantList = class(ALinkedList)
  public
    { Construct a new variant list that contains the specified values.

      This method calls @link(AVariantList.pushSeveral) to insert the values in
      @code(values) into the list after the new instance has been constructed
      and initialized.  If @code(values) is empty, then an empty list is
      created by this routine.
    }
    constructor withValues(const values: array of const); virtual;

    { Construct a new variant list by expanding the values provided in
      @code(thisList).

      If @code(thisDelimiter) is provided by the caller, then it is used to
      split the values provided in @code(thisList); otherwise, the delimiter
      specified by @link(slstDefaultDelimiter) is used.  See
      @link(AVariantList.explode) for more information.

      If @code(thisList) is an empty string, then an empty list is created by
      this routine.
    }
    constructor from(const thisList: string; const thisDelimiter: string = '');
      virtual;

    { Initializer }
    function init: boolean; override;

    { Adds @code(thisValue) to the end of the list.

      This method constructs a new instance of @link(AVariantListItem) and then
      calls @link(ALinkedList.addItem) to add the item to the list.

      @return(The index of the new item.  This index is not guaranteed to
      remain the same for this item if other items are added or removed at
      locations PRIOR to the index returned.)
    }
    function add(const thisValue: Variant): TNodeAbsoluteIndex; virtual;

    { Add several values to the end of the list.

      This method calls @link(TVarRecToVariant) in order to convert each of the
      values specified in @code(values) to a @code(Variant) value.  It then
      calls @link(AVariantList.add) to add the value to the list.

      @return(The total number of items added to the list.)
    }
    function addSeveral(const values: array of const): TCensus; virtual;

    { Add the items specified in @code(thisList) to the list.

      This method assumes that @code(thisList) contains one or more values that
      are separated by a delimiter of some kind (e.g., a comma).  It uses the
      value of @code(delimiter) to split the string into its component values;
      these values are then added as individual items to the list.  If
      @code(delimiter) is not supplied by the caller, or if it is an empty
      string, then the value of @link(llstDefaultDelimiter) is used.

      @return(The total number of items added to the list.)
    }
    function explode(thisList: string;
      delimiter: string = ''): longword; override;

    { Pushes the specified variant value onto the list, as the last item in the
      list.  Calling @link(AVariantList.pop) immediately after this routine will
      return @code(thisValue).

      This method constructs a new instance of @link(AVariantListItem) and then
      calls @link(ALinkedList.pushItem) to push the item onto the list.
    }
    procedure push(const thisValue: Variant); virtual;

    { Push several values onto the list.

      This method calls @link(TVarRecToVariant) in order to convert each of the
      values specified in @code(values) to a @code(Variant) value.  It then
      calls @link(AVariantList.push) to add the value to the list.
    }
    procedure pushSeveral(const values: array of const); virtual;

    { Pops the last item from the list.  This will be the value most recently
      added to the list by a call to @link(AVariantList.add) or
      @link(AVariantList.push).

      If the list is empty, this routine returns an empty string.
    }
    function pop: Variant; virtual;

    { Inserts the specified variant value at the given @code(index).  If an item
      already exists at that index, it is "bumped aside" in favor of the new
      item.

      This method constructs a new instance of @link(AVariantListItem) and then
      calls @link(ALinkedList.insertItemAt) to insert the item into the list.

      The first item in the list is always at index zero (0).  The last item
      in the list is always at index (@link(ALinkedList.census) @code(- 1)).
      If @code(index) specifies a value that is greater than the number of items
      in the list, then @code(thisString) is inserted at the end of the list.
    }
    procedure insertAt(const index: TNodeRelativeIndex;
      const thisValue: Variant); virtual;

    { Performs a recursive search to determine whether or not the specified
      value exists within the list.  For very large lists, this search can
      be inefficient (and can cause stack overflows, due to the recursive
      nature of the search).

      This method calls @link(AVariantListItem.find) on the last item in the
      list, which causes the entire list to be searched recursively.

      @return(@true if @code(thisValue) is found in the list; @false if not.)
    }
    function hasValue(const thisValue: Variant): boolean; virtual;

    { Performs a linear search to find the index of the specified value.  The
      search will begin at @code(afterIndex).

      Because this method performs a linear search, it can be inefficient on
      very large lists.

      @return(The index of the specified value, if found; @code(-1) otherwise.
      The index returned by this routine will always be an offset relative to
      the beginning of the list (i.e., a positive integer) if the string is
      found.)
    }
    function indexOf(const thisValue: Variant;
      const afterIndex: TNodeRelativeIndex = 0): longint; virtual;

  // Properties ----------------------------------------------------------------
    { Retrieve the value of the item at the specified index.

      This method calls @link(ALinkedList.itemAt) to retrieve the
      @link(AVariantListItem) instance at the specified index; it then returns
      the value of @link(AVariantListItem.value).

      Index zero (0) represents the first node in the list, while index
      (@link(ALinkedList.census) @code(- 1)) represents the last node in the
      list.  If @code(index) specifies a value that is greater than the number
      of nodes in the list, then this routine will remove the last item in
      the list.

      If an invalid index is passed to this routine, it will return an empty
      string.
    }
    function item(const thisIndex: TNodeRelativeIndex): Variant; virtual;

    { Set the value of the item at the specified index.

      This method calls @link(ALinkedList.itemAt) to retrieve the
      @link(AVariantListItem) instance at the specified index; it then calls
      @link(AVariantListItem.setValue) on that instance.

      @return(The previous value of the string stored at the specified index,
      if any.  An empty string will be returned if an invalid index is passed
      to this routine.)
    }
    function setItem(const thisIndex: TNodeRelativeIndex;
      const thisValue: Variant): Variant; virtual;
end;

// Utility functions ***********************************************************
{ This function converts the value in @code(value) to a @code(Variant).

  This function converts @code(value) as follows:
  @unorderedList(
    @item(If @code(value.vtype) is @code(vtBoolean), the resulting variant is
      cast as a @code(WordBool).)
    @item(If @code(value.vtype) is @code(vtInteger), the resulting variant is
      cast as a @code(longint).)
    @item(If @code(value.vtype) is @code(vtInt64), the resulting variant is
      cast as a @code(int64).)
    @item(If @code(value.vtype) is @code(vtQWord), the resulting variant is
      cast as a @code(qword).)
    @item(If @code(value.vtype) is @code(vtExtended), the resulting variant is
      cast as a @code(double), with some potential loss of precision, since
      @code(Variant)s cannot store @code(extended) values.)
    @item(If @code(value.vtype) is @code(vtCurrency), the resulting variant is
      cast as a @code(currency) type.)
    @item(If @code(value.vtype) is @code(vtChar), @code(vtWideChar),
      @code(vtString), @code(vtPChar), @code(vtPWideChar), @code(vtAnsiString),
      @code(vtWideString), or @code(vtUnicodeString), the resulting variant is
      cast as an @code(AnsiString).)
    @item(If @code(value.vtype) is @code(vtObject), @code(vtClass), or
      @code(vtInterface), the resulting variant is cast as a @code(pointer).)
    @item(If @code(value.vtype) is @code(vtVariant), the resulting variant is
      set to the value of @code(value.vVariant).)
  )

  In all other cases, the resulting variant is set to @code(Variants.Null).
}
function TVarRecToVariant(const value: TVarRec): Variant;
{$ENDIF}
{$IFDEF ReadImplementation}
// AVariantListItem ************************************************************
{* Constructs and initializes new instance of a variant list item.

   \return A new variant list item instance on success; #nil on failure.
*}
constructor AVariantListItem.withValue(const thisValue: Variant);

begin
  inherited new;
  myValue := thisValue;
end;

{* Initializes a new instance of a variant list item.  This routine sets
   AVariantListItem.value to Null and then calls the inherited initializer.

   \return #true if initialization was successful; #false if not.
*}
function AVariantListItem.init: boolean;

begin
  myValue := Variants.null;
  result := inherited init;
end;

// Search the node, its children, and older siblings for the specified value
function AVariantListItem.find(const thisValue: Variant): AVariantListItem;

begin
  Result := nil;

  // Check ourselves
  if myValue = thisValue then
    Result := Self

  // Recurse through older siblings
  else if MyPrevious <> nil then
    Result := AVariantListItem(MyPrevious).find(thisValue);

  // Recurse through children
  if (Result = nil) and (MyChild <> nil) then
    Result := AVariantListItem(MyChild).find(thisValue);
end;

// Construct a shallow copy of the given object
function AVariantListItem.shallowCopyFrom(const Other: AnObject): boolean;

begin
  // Call the inherited routine first
  result := inherited shallowCopyFrom(Other);

  // Check to see whether the other item is compatible with us
  if not(Other is AVariantListItem) then
    result := false;

  if result then
  begin
    myValue := AVariantListItem(Other).value;
  end;
end;

// Calculate the number of bytes required to stream the node
function AVariantListItem.selfStreamingLength: TStreamIOSize;

begin
  // Call the inherited routine first
  result := inherited selfStreamingLength;

  // Add ourselves
  inc(result, sizeof(TVarData));
end;

// Write the node, and just the node, to the specified stream
function AVariantListItem.writeSelfTo(const Dest: AStream): TStreamIOSize;

begin
  result := 0;

  if Dest <> nil then
    result := Dest.write(sizeof(TVarData), TVarData(myValue));

  result := result + inherited writeSelfTo(Dest);
end;

// CanStream implementation ----------------------------------------------------
{* Reads the list item and its children from the specified stream.

   \return The total number of bytes read from \p Source.  This value will be
   zero (0) if \p Source is #nil.
*}
function AVariantListItem.readFrom(const Source: AStream): TStreamIOSize;

begin
  result := 0;

  if Source <> nil then
    result := Source.read(sizeof(TVarData), TVarData(myValue));

  result := result + inherited readFrom(Source);
end;

// CanPrint implementation -----------------------------------------------------
{* Constructs a string representation of the item.  This routine checks the
   value of AVariantListItem.value; if it is not an empty string, that value is
   returned.  Otherwise, this routine calls the inherited function
   (ANode.toString()).

   \return A string representation of the item, as described above.
*}
function AVariantListItem.toString: AnsiString;

begin
  result := Variants.varToStr(myValue);
end;

// Properties ------------------------------------------------------------------
/// Retrieve the value of the item
function AVariantListItem.value: Variant;

begin
  result := myValue;
end;

/// Set the value of the item
function AVariantListItem.setValue(const newValue: Variant): Variant;

begin
  result := myValue;
  myValue := newValue;
end;

// AVariantList ****************************************************************
// Construct a variant list with the specified values
constructor AVariantList.withValues(const values: array of const);

begin
  inherited new;

  Self.pushSeveral(values);
end;

{* Constructs and initializes a new instance of a variant list.  This routine
   sets AVariantList.delimiter to \p thisDelimiter and then calls
   AVariantList.explode() to process \p thisList.

   \return A new instance of AVariantList on success; #nil on failure.
*}
constructor AVariantList.from(const thisList: string;
  const thisDelimiter: string = '');

begin
  inherited new;

  if thisList <> '' then
    Self.explode(thisList, thisDelimiter);
end;

// Initializer
function AVariantList.init: boolean;

begin
  result := inherited init;

  if result then
  begin
    MyItemType := AVariantListItem;
  end;
end;

{* Adds the specified value to the list.  This routine calls
   AList.addItem().

   \return The index of the new item.  This index is not guaranteed to be
   static, as a call to AVariantList.insertAt() that specifies a lower index
   will push all higher indexes "down" the list.
*}
function AVariantList.add(const thisValue: Variant): TNodeAbsoluteIndex;

begin
  result := Self.addItem(AVariantListItem.withValue(thisValue));
end;

// Add several items to the list
function AVariantList.addSeveral(const values: array of const): TCensus;

var
  // Loop counter
  i: longword;

begin
  result := 0;

  if System.length(values) = 0 then
    exit;

  for i := 0 to (System.high(values)) do
  begin
    Self.add(TVarRecToVariant(values[i]));
    inc(result);
  end;
end;

{* Builds the list from a delimited list of items.  This routine uses the value
   of delimiter to determine where to separate the list items.

   \return The number of items parsed from the list.  This value will be zero
   (0) if \p thisList is an empty string.
*}
function AVariantList.explode(thisList: string;
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

{* Pushes the specified value onto the end of the list.  This routine calls
   AList.pushItem().
*}
procedure AVariantList.push(const thisValue: Variant);

begin
  Self.pushItem(AVariantListItem.withValue(thisValue));
end;

// Push several values onto the list
procedure AVariantList.pushSeveral(const values: array of const);

var
  // Loop counter
  i: longword;

begin
  if System.length(values) = 0 then
    exit;

  for i := 0 to (System.high(values)) do
    Self.push(TVarRecToVariant(values[i]));
end;

{* Pops the last value from the end of the list.  This routine calls
   AList.popItem().

   \return The last value from the list.  If there are no variants in the list,
   this routine will return Null.
*}
function AVariantList.pop: Variant;

var
  // Refers to the item retrieved from the list
  ThisItem: AVariantListItem;

begin
  result := '';
  ThisItem := AVariantListItem(Self.popItem);

  if ThisItem <> nil then
  begin
    result := ThisItem.value;
    ThisItem.free;
  end;
end;

{* Inserts the specified value at the specified index.  This routine calls
   AList.insertItemAt().
*}
procedure AVariantList.insertAt(const index: TNodeRelativeIndex;
  const thisValue: Variant);

begin
  Self.insertItemAt(index, AVariantListItem.withValue(thisValue));
end;

// Determine whether the specified text is in the list
function AVariantList.hasValue(const thisValue: Variant): boolean;

begin
  result := false;

  if MyLastItem <> nil then
    result := AVariantListItem(MyLastItem).find(thisValue) <> nil;
end;

// Find the index of the specified text
function AVariantList.indexOf(const thisValue: Variant;
  const afterIndex: TNodeRelativeIndex = 0): longint;

var
  // Refers to the current item
  ThisItem: AVariantListItem;
  // Stores the current index
  currentIndex: TNodeRelativeIndex;

begin
  result := -1;

  ThisItem := AVariantListItem(Self.itemAt(afterIndex));

  currentIndex := afterIndex;

  { Ensure we only use indices relative to the beginning of the list }
  if currentIndex < 0 then
    inc(currentIndex, myCensus + 1);

  if currentIndex < 0 then
    currentIndex := 0;

  while ThisItem <> nil do
  begin
    if ThisItem.value = thisValue then
    begin
      result := currentIndex;
      break;
    end;

    ThisItem := AVariantListItem(ThisItem.next);
    inc(currentIndex);
  end;
end;

// Properties ------------------------------------------------------------------
{* Fetches the value at the specified index.

   \return The value at \p thisIndex.  If \p thisIndex is out of range, this
   routine will return Null.
*}
function AVariantList.item(const thisIndex: TNodeRelativeIndex): Variant;

var
  // Refers to the item at the specified index
  ThisItem: AVariantListItem;

begin
  result := Variants.null;

  ThisItem := AVariantListItem(Self.itemAt(thisIndex));
  if ThisItem <> nil then
    result := ThisItem.value;
end;

{* Sets the value at the specified index.  If \p thisIndex is out of range, this
   routine will add a new item to the end of the list that contains the
   specified value
*}
function AVariantList.setItem(const thisIndex: TNodeRelativeIndex;
  const thisValue: Variant): Variant;

var
  // Refers to the item at the specified index
  ThisItem: AVariantListItem;

begin
  Result := Variants.null;

  // Fetch the item
  ThisItem := AVariantListItem(Self.itemAt(thisIndex));
  // If the item was found, then set the value
  if ThisItem <> nil then
  begin
    Result := ThisItem.value;
    ThisItem.setValue(thisValue);
  end

  // Otherwise, insert the item at the end of the list
  else begin
    ThisItem := AVariantListItem.withValue(thisValue);
    if ThisItem <> nil then
      Self.pushItem(ThisItem);
  end;
end;

// Utility functions ***********************************************************
// Convert a TVarRec to a Variant
function TVarRecToVariant(const value: TVarRec): Variant;

begin
  result := Variants.null;

  case value.vType of
    vtInteger:
      result := value.vInteger;

    vtBoolean:
      result := value.vBoolean;

    vtChar:
      result := value.vChar;

    vtWideChar:
      result := System.wideCharLenToString(@value.vWideChar, 1);

    vtExtended:
      result := double(value.vExtended^);

    vtString:
      result := value.vString^;

    vtPointer:
      result := value.vPointer;

    vtPChar:
      result := value.vPChar^;

    vtObject:
      result := pointer(value.VObject);

    vtClass:
      result := pointer(value.VClass);

    vtPWideChar:
      result := System.wideCharToString(value.vpWideChar);

    vtAnsiString:
      result := AnsiString(value.vAnsiString);

    vtCurrency:
      result := value.vCurrency^;

    vtVariant:
      result := value.vVariant^;

    vtInterface:
      result := value.vInterface;

    vtWideString:
      result := WideString(value.vWideString);

    vtInt64:
      result := value.vInt64^;

    vtUnicodeString:
      result := UnicodeString(value.vUnicodeString);

    vtQWord:
      result := value.vQWord^;
  end;
end;
{$ENDIF}

