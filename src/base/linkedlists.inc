{ linkedlists.inc: Doubly-linked lists of nodes.

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
{ This class represents a doubly-linked list of nodes.  It supports accessing
  the list as if it was an indexed array or as a stack.  The list can manage
  instances of @link(ANode) and its descendants.
}
type ALinkedList = class (APrintingObject, CanStream)
  protected
    { Refers to the node type managed by the list }
    MyItemType: ANodeClass;
    { Refers to the first item in the list }
    MyFirstItem: ANode;
    { Refers to the last item in the list }
    MyLastItem: ANode;
    { Stores the current number of items in the list }
    myCensus: TCensus;

  public
    { Initializer }
    function init: boolean; override;

    { Destroy the list and its nodes.

      This method calls @code(TObject.free) on its last node, which will cause
      all nodes in the list to be freed.
    }
    destructor destroy; override;

    { Push @code(ThisItem) onto the list, as the last node in the list.

      A call to @link(ALinkedList.LastItem) after this routine finishes will
      return @code(ThisItem).
    }
    procedure pushItem(const ThisItem: ANode); virtual;

    { Pop the last item from the list. }
    function popItem: ANode; virtual;

    { Retrieve the node at the specified @code(index) within the list.

      This method performs a sequential search of each node in the list; for
      very large lists, therefore, it can be inefficient.

      Index zero (0) represents the first node in the list, while index
      (@link(ALinkedList.census) @code(-1)) represents the last node in the
      list.  The first and last nodes in the list can also be obtained by
      calling @link(ALinkedList.FirstItem) and @link(ALinkedList.LastItem),
      respectively.

      If @code(index) specifies a value that is greater than the number of
      nodes in the list, then this routine will return the last node in the
      list.
    }
    function itemAt(index: TNodeRelativeIndex): ANode; virtual;

    { Insert @code(ThisItem) at the specified @code(index) within the list.

      If a node already exists in the specified position, it "bumped down" to
      make room for the new item.

      Index zero (0) represents the first node in the list, while index
      (@link(ALinkedList.census) @code(-1)) represents the last node in the
      list.  If @code(index) specifies a value that is greater than the number
      of nodes in the list, then this routine will insert @code(ThisItem)
      at the end of the list.
    }
    procedure insertItemAt(index: TNodeRelativeIndex; const ThisItem: ANode);
      virtual;

    { Adds @code(thisItem) to the end of the list.

      This function is nearly identical to @link(ALinkedList.pushItem), except
      that it returns the index of the node added to the list.  This index
      is not guaranteed to always represent the item, however; as nodes are
      added and deleted from the list, it is likely that their indices will
      change.
    }
    function addItem(const ThisItem: ANode): TNodeAbsoluteIndex; virtual;

    { Condenses the values in the list into a delimited string.

      This method uses the value of @code(delimiter) to separate the values in
      the list, which are all condensed into a single string that is returned to
      the caller.  If @code(delimiter) is not supplied by the caller, or if it
      is an empty string, then the value of @link(lstDefaultDelimiter) is
      used.

      The base behavior of this method is to iterate through each node in the
      list and call @link(ANode.toString) on them.  The values returned are
      collected into a single (potentially long) string, wherein each value is
      separated from the next by @code(delimiter).  Descendant classes may
      override this behavior.
    }
    function implode(delimiter: string = ''): AnsiString; virtual;

    { Add the items specified in @code(thisList) to the list.

      This method assumes that @code(thisList) contains one or more values that
      are separated by a delimiter of some kind (e.g., a comma).  It uses the
      value of @code(delimiter) to split the string into its component values;
      these values are then added as individual items to the list.  If
      @code(delimiter) is not supplied, or if it is an empty string, then
      the value of @link(llstDefaultDelimiter) is used.

      This method does nothing in the base instance of @link(ALinkedList); it is
      defined as a counterpoint to @link(ALinkedList.implode) and to ensure
      consistent behavior across descendants of @link(ALinkedList).

      @return(The total number of items added to the list.)
    }
    function explode(thisList: AnsiString;
      delimiter: string = ''): TCensus; virtual;

    { Removes the node at the specified @code(index) from the list.

      Index zero (0) represents the first node in the list, while index
      (@link(ALinkedList.census) @code(-1)) represents the last node in the
      list.  If @code(index) specifies a value that is greater than the number
      of nodes in the list, then this routine will remove the last item in
      the list.

      The node is removed, but is not freed.  It is the caller's responsibility
      to free the node when it is no longer required.
    }
    function removeItemAt(const index: TNodeRelativeIndex): ANode; virtual;

    { Deletes the node at the specified @code(index) from the list.

      This method calls @link(ALinkedList.removeItemAt) to remove the specified
      node from the list; it then calls @code(TObject.free) on that node.

      Index zero (0) represents the first node in the list, while index
      (@link(ALinkedList.census) @code(-1)) represents the last node in the
      list.  If @code(index) specifies a value that is greater than the number
      of nodes in the list, then this routine will remove the last item in
      the list.

      If the node has any children, they will be also be deleted.  To prevent
      this from happening, you should call @link(ALinkedList.removeItemAt) to
      obtain a reference to the node, then call @link(ANode.adoptChildrenOf) to
      reassign its children to another node.  You may then free the node.

      @return(@true if the node was found and was deleted; @false otherwise).)
    }
    function deleteItemAt(const index: TNodeRelativeIndex): boolean; virtual;

    { Construct a shallow copy of the other object.

      This method overrides the behavior inherited from
      @link(AnObject.shallowCopyFrom): it calls that method, then checks to
      see whether @code(Other) is an instance of @link(ALinkedList).  If so,
      it creates a copy of the items in the list by calling
      @link(AnObject.shallowCopyFrom) on each.

      The new list items will be of the same type as
      @link(ALinkedList.ItemType).

      Note that the behavior of this method makes it possible to insert a copy
      of a list into an existing list, as no existing list items are deleted
      by this routine.
    }
    function shallowCopyFrom(const Other: AnObject): boolean; override;

    // CanStream implementation ------------------------------------------------
    { Calculate the number of bytes required to stream the list and its nodes.

      This method calls @link(ANode.streamingLength) and then adds the number
      of bytes required to stream @link(ALinkedList.census) to the result, which
      is then returned to the caller.
    }
    function streamingLength: TStreamIOSize; virtual;

    { Write the list and its nodes to the specified stream.

      This method first writes the value of @link(ALinkedList.census) to
      @code(Dest); then it calls @link(ANode.writeTo) on the last node in the
      list, which recursively causes all nodes in the list to be written to
      the stream.

      @return(The total number of bytes written to @code(Dest).)
    }
    function writeTo(const Dest: AStream): TStreamIOSize; virtual;

    { Read the list and its nodes from the specified stream.

      This method first reads the value of @link(ALinkedList.census) from
      @code(Source); it then constructs an instance of
      @link(ALinkedList.ItemType) for each entry in the stream and calls
      @link(ANode.readFrom) on each.

      @return(The total number of bytes read from @code(Source).)
    }
    function readFrom(const Source: AStream): TStreamIOSize; virtual;

    // CanPrint implementation -------------------------------------------------
    { Construct and return a string representation of the list.

      The base implementation of this method within @code(ALinkedList)
      returns a string that contains the name of the class and the current
      value of @link(ALinkedList.census).  The format of this string is
      controlled by @link(llstStringRepresentationSingular) or
      @link(llstStringRepresentationPlural), depending upon the number of nodes
      in the list.
    }
    function toString: AnsiString; override;

    { Print a string representation of the list and its nodes to the specified
      stream.

      This method first prints the string representation returned by a call to
      @link(ALinkedList.toString), then calls @link(ANode.printTo) on its
      last node, which recursively causes the entire list to be printed to
      @code(Dest).

      @code(prefix) and @code(suffix) are used directly by this method and are
      not passed to the nodes; instead, this routine passes
      @link(llstDefaultNodePrintingPrefix) and
      @link(llstDefaultNodePrintingSuffix).

      @return(The total number of bytes printed to @code(Dest).)
    }
    function printTo(const Dest: ATextOutputStream;
      prefix: AnsiString = '';
      suffix: AnsiString = ''): TStreamIOSize; override;

  // Properties ----------------------------------------------------------------
    { Retrieve the type of node managed by the list.  This will be a reference
      to @link(ANode) or one of its descendants.
    }
    function ItemType: ANodeClass; virtual;

    { Retrieve the total number of nodes in the list.

      The value returned by this function includes the total number of
      top-level nodes in the list; it does not account for children of those
      nodes.  It is assumed -- but not required -- that instances of
      @link(ALinkedList) will only contain top-level nodes.
    }
    function census: TCensus; virtual;

    { Retrieve a reference to the first node in the list.

      If the list has no nodes, this routine will return @nil.  The reference
      returned by this routine should NOT be freed by the caller.
    }
    function FirstItem: ANode; virtual;

    { Retrieve a reference to the last node in the list.

      If the list has no nodes, this routine will return @nil.  The reference
      returned by this routine should NOT be freed by the caller.
    }
    function LastItem: ANode; virtual;
end;
{$ENDIF}
{$IFDEF ReadResourceStrings}
  { This string controls how a string representation of @link(ALinkedList) is
    constructed by @link(ALinkedList.toString).  It is used when the list has
    exactly one node.

    The string placeholder is filled with the display name of the class, as
    obtained by a call to @link(APrintingObject.displayName).  The integer
    placeholder will be filled with the value of @link(ALinkedList.census),
    which will always be one (1) when this string is used.
  }
  llstStringRepresentationSingular = '%s that contains %d node';

  { This string controls how a string representation of @link(ALinkedList) is
    constructed by @link(ALinkedList.toString).  It is used when the list does
    not have exactly one node (it either has no nodes, or has more than one
    node).

    The string placeholder is filled with the display name of the class, as
    obtained by a call to @link(APrintingObject.displayName).  The integer
    placeholder will be filled with the value of @link(ALinkedList.census).
  }
  llstStringRepresentationPlural = '%s that contains %d nodes';

  { This string is used by @link(ALinkedList.printTo).  It serves as the
    @code(preffix) passed when the nodes in the list are to be printed to a
    given stream by means of @link(ANode.printTo).

    The default value is a tab character, which ensures that each list item
    is indented below the description of the list itself.
  }
  llstDefaultNodePrintingPrefix = #9;

  { This string is used by @link(ALinkedList.printTo).  It serves as the
    @code(suffix) passed when the nodes in the list are to be printed to a given
    stream by means of @link(ANode.printTo).

    The default value is the same as @code(System.lineEnding), ensuring that
    each list item occupies a line of its own.
  }
  llstDefaultNodePrintingSuffix = lineEnding;

  { This string is used as the default delimiter by @link(AList.explode) and
    @link(AList.implode) when no other delimiter is supplied.
  }
  llstDefaultDelimiter = ',';

  { This string can be passed to @link(AList.implode) to cause its output to
    be formatted into a single column composed of many rows instead of a
    lengthy string.

    This string is defined for convenience and is not used by default.
  }
  llstDelimiterTabbedColumn = lineEnding + #9;
{$ENDIF}
{$IFDEF ReadImplementation}
// ALinkedList *****************************************************************
{* Initializes a new instance of AList.  This routine sets AList::FirstItem()
   and AList::LastItem() to #nil; it also sets AList::census() to zero (0).  The
   inherited routine is then called.  In the default implementation, therefore,
   this routine always returns #true.

   \return #true if initialization was successful; #false if not.
*}
function ALinkedList.init: boolean;

begin
  MyItemType := ANode;

  // No items, as yet
  MyFirstItem := nil;
  MyLastItem := nil;
  myCensus := 0;
  result := inherited init;
end;

{* Destroys the list and all items therein.  This routine calls ANode::free() on
   the last item in the list; that routine recursively calls ANode::free() on
   all other items.
*}
destructor ALinkedList.destroy;

begin
  // Destroy all list items
  if MyLastItem <> nil then
    MyLastItem.free;

  inherited destroy;
end;

{* Adds \p ThisItem to the end of the list.  This call allows the list to be
   used like a stack: the most recent item pushed onto the list will be the
   one returned by AList::popItem().  If \p ThisItem is #nil, then this routine
   does nothing.
*}
procedure ALinkedList.pushItem(const ThisItem: ANode);

begin
  if ThisItem <> nil then
  begin
    if MyLastItem <> nil then
    begin
      // Add the last item
      MyLastItem.append(ThisItem);
      MyLastItem := ThisItem;
    end

    else
      MyLastItem := ThisItem;

    if MyFirstItem = nil then
      MyFirstItem := MyLastItem;

    inc(myCensus);
  end;
end;

{* Removes and returns the last item in the list.  This routine allows the list
   to be used like a stack, in which the most recent item added to the list
   by ALinkedList::pushItem() is the first one returned by this call.

   \return An ANode instance that contains the last item in the list.
   ALinkedList::LastItem() is updated to reflect the removal of this item from
   the list.  If there are no items in the list, this routine returns #nil.
*}
function ALinkedList.PopItem: ANode;

begin
  Result := nil;

  if MyLastItem <> nil then
  begin
    Result := MyLastItem;
    MyLastItem := MyLastItem.Previous;

    // Detach the last item from the list
    Result.detach;
    dec(myCensus);
  end;

  if MyLastItem = nil then
    MyFirstItem := nil;
end;

{* Retrieves the item at \p index.  This routine performs a brute-force
   search of the list, starting with AList::FirstItem() and progressing through
   each successive item.  For large lists, therefore, it is not the most
   efficient way to find a specific item.

   The first item in the list resides at index zero (0).

   \return The ANode instance that resides as the specified \p index.
*}
function ALinkedList.itemAt(index: TNodeRelativeIndex): ANode;

begin
  Result := nil;

  // Check the index
  if index < 0 then
    inc(index, myCensus + 1);

  if index < 0 then
    index := 0;

  if index = 0 then
    Result := MyFirstItem

  else if index >= myCensus then
    Result := MyLastItem

  else
    // Start with the first item
    Result := MyFirstItem.SiblingAtAbsolutePosition(index);
end;

{* Inserts \p ThisItem into the list at the specified \p index.  If \p index
   is one higher than that of the last item in the list, this routine is
   functionally equivalent to AList::pushItem().  Otherwise, the existing item
   at \p index is "moved down" to make room form \p ThisItem.

   If \p ThisItem is #nil, this routine does nothing.
*}
procedure ALinkedList.insertItemAt(index: TNodeRelativeIndex;
  const ThisItem: ANode);

var
  // Refers to the item being bumped from position
  BumpedItem: ANode;

begin
  if ThisItem <> nil then
  begin
    if index < 0 then
      inc(index, myCensus + 1);

    if index < 0 then
      index := 0;

    // Push the item if we are to add it to the end of the list
    if index >= myCensus then
      Self.pushItem(ThisItem)

    else begin
      // Insert the item
      BumpedItem := Self.ItemAt(index);

      if BumpedItem <> nil then
      begin
        BumpedItem.prepend(ThisItem);
        // Increment the items count
        inc(myCensus);
      end;

      if BumpedItem = MyFirstItem then
        MyFirstItem := ThisItem;
    end;
  end;
end;

{* Adds \p ThisItem to the end of the list and returns its index.  This routine
   is essentially the same as AList::pushItem(), except that it returns the
   index of the added item.  This index is not guaranteed to be static, however;
   if another item is inserted into or removed from the list before \p ThisItem,
   the index of \p ThisItem will change.

   \return The index of \p ThisItem after it has been inserted into the list,
   which indicates the position of \p ThisItem within the list.  This routine
   will return zero (0) if \p ThisItem is #nil.
*}
function ALinkedList.addItem(const ThisItem: ANode): TNodeAbsoluteIndex;

begin
  result := 0;

  if ThisItem <> nil then
  begin
    if MyLastItem <> nil then
    begin
      // Add the last item
      MyLastItem.append(ThisItem);
      MyLastItem := ThisItem;
    end

    else
      MyLastItem := ThisItem;

    if MyFirstItem = nil then
      MyFirstItem := MyLastItem;

    result := myCensus;

    inc(myCensus);
  end;
end;

{* Removes the item at the specified \p index from the list.  The item is
   returned to the caller; it is not destroyed.

   \return The item at the specified \p index.
*}
function ALinkedList.RemoveItemAt(const index: TNodeRelativeIndex): ANode;

begin
  Result := nil;

  // Fetch the item
  Result := Self.ItemAt(index);

  if Result <> nil then begin
    if Result = MyFirstItem then
      MyFirstItem := Result.Next;

    if Result = MyLastItem then
      MyLastItem := Result.Previous;

    // Detach the item
    Result.detach;
    // Decrement our items count
    dec(myCensus);
  end;
end;

// Delete the item at the specified index
function ALinkedList.deleteItemAt(const index: TNodeRelativeIndex): boolean;

var
  // Refers to the item in question
  ThisItem: ANode;

begin
  result := false;

  ThisItem := Self.RemoveItemAt(index);

  if ThisItem <> nil then
  begin
    ThisItem.free;
    result := true;
  end;
end;

{* Condenses the list into a delimited string.  This string will contain each
   value in the list, separated by the delimiter specified by
   delimiter.  If there are no items in the list, an empty string
   is returned.

   \return A string, as described above.
*}
function ALinkedList.implode(delimiter: string = ''): AnsiString;

var
  // Refers to an item from the list
  ThisItem: ANode;

begin
  result := '';

  if delimiter = '' then
    delimiter := llstDefaultDelimiter;

  ThisItem := Self.FirstItem;

  while ThisItem <> nil do
  begin
    result := result + ThisItem.toString;

    if ThisItem.Next <> nil then
      result := result + delimiter;

    ThisItem := ThisItem.Next;
  end;
end;

// Expand the list from a delimited string; does nothing here
function ALinkedList.explode(thisList: AnsiString;
  delimiter: string): TCensus;

begin
  // Does nothing; descendant classes must implement this method
  result := 0;
end;

// Construct a shallow copy of the other object
function ALinkedList.shallowCopyFrom(const Other: AnObject): boolean;

var
  // Used to traverse the specified list
  ThisItem: ANode;

begin
  // Call the inherited method first
  result := inherited shallowCopyFrom(Other);

  // Check to see whether the other object is compatible with us
  if not(Other is ALinkedList) then
    result := false;

  if result then
  begin
    ThisItem := ALinkedList(Other).FirstItem;

    while ThisItem <> nil do
    begin
      Self.addItem(ANode(MyItemType.shallowCopyOf(ThisItem)));
      ThisItem := ThisItem.Next;
    end;
  end;
end;

// CanStream implementation ----------------------------------------------------
// Calculate the number of bytes required to stream the list
function ALinkedList.streamingLength: TStreamIOSize;

begin
  result := sizeof(myCensus);

  // Include our nodes
  if MyLastItem <> nil then
    inc(result, MyLastItem.streamingLength);
end;

{* Writes the list and its items to \p Dest.  This routine first writes the
   number of items in the list, then calls ANode::writeTo() on the last item in
   the last.  That function recursively writes all items in the list to the
   stream.

   \return The total number of bytes written to \p Dest.  This value will be
   zero (0) if \p Dest is #nil.
*}
function ALinkedList.writeTo(const Dest: AStream): TStreamIOSize;

begin
  result := 0;

  if Dest <> nil then
  begin
    // Write the number of items to the stream
    result := Dest.write(sizeof(myCensus), myCensus);

    // Write each item
    if MyLastItem <> nil then
      result := result + MyLastItem.writeTo(Dest);
  end;
end;

// Read the list and its items from the given stream
function ALinkedList.readFrom(const Source: AStream): TStreamIOSize;

var
  // Stores the number of items in the list, as read from the stream
  entryCount: TCensus;
  // Refers to a new item created for the list
  NewItem: ANode;

begin
  result := 0;

  if Source = nil then
    exit;

  { Read the entry count }
  entryCount := 0;
  result := Source.read(sizeof(entryCount), entryCount);

  while entryCount > 0 do
  begin
    NewItem := MyItemType.new;
    result := result + NewItem.readFrom(Source);

    Self.pushItem(NewItem);
  end;
end;

// CanPrint implementation -----------------------------------------------------
{ Constructs a string representation of the list. }
function ALinkedList.toString: AnsiString;

begin
  if myCensus <> 1 then
    result := format(llstStringRepresentationPlural, [
      Self.displayName, myCensus
    ])

  else
    result := format(llstStringRepresentationSingular, [
      Self.displayName, myCensus
    ]);
end;

{* Prints a string representation of the list and its items to \p Dest.  The
   representation used for the list is that produced by AList::toString().
   This routine calls ANode::printTo() on the last item in the list; that
   routine recursively prints a string representation for all other items in
   the list.

   \return The total number of bytes written to \p Dest.  This value will be
   zero (0) if \p Dest is #nil.
*}
function ALinkedList.printTo(const Dest: ATextOutputStream;
  prefix: AnsiString = '';
  suffix: AnsiString = ''): TStreamIOSize;

begin
  result := 0;

  if Dest <> nil then
  begin
    result := Dest.printEach([prefix, Self.toString, suffix]);

    // Print each item
    if MyLastItem <> nil then
      result := result + MyLastItem.printTo(Dest, llstDefaultNodePrintingPrefix,
        llstDefaultNodePrintingSuffix);
  end;
end;

// Properties ------------------------------------------------------------------
// Retrieve the item type managed by the list
function ALinkedList.ItemType: ANodeClass;

begin
  Result := MyItemType;
end;

/// Retrieves the total number of items in the list
function ALinkedList.census: TCensus;

begin
  result := myCensus;
end;

/// Retrieves the first node in the list
function ALinkedList.FirstItem: ANode;

begin
  result := MyFirstItem;
end;

/// Retrieves the last node in the list
function ALinkedList.LastItem: ANode;

begin
  result := MyLastItem;
end;
{$ENDIF}

