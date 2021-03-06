{ nodes.inc: Doubly-linked, hierarchical nodes for linked lists.

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
{ This type is used to represent an absolute index, which is a way of referring
  to the position of @link(ANode a node) within a sequence of nodes, relative to
  the beginning of the sequence.  The index is zero-based, with the first node
  in the sequence always positioned at index zero (@code(0)).

  This type is defined to make code flexible and easy to change.
}
type TNodeAbsoluteIndex = longword;

{ This type is used to represent a relative index, which is a way of referring
  to the position of @link(ANode a node) within a sequence of nodes, relative to
  a specific node.  Values that are less than zero refer to a node which occurs
  prior to the node in question, while values that are greater than zero refer
  to a node which occurs after the node in question.  Values of zero (@code(0))
  refer to the node in question.

  This type is defined to make code flexible and easy to change.
}
type TNodeRelativeIndex = longint;

{ This type is used to represent a census, which is a way of storing the count
  of @link(ANode nodes) within a list of nodes.

  This type is defined to make code flexible and easy to change.
}
type TCensus = longword;

// ANode ***********************************************************************
{ This class represents a single object in a hierarchical linked list.  Each
  node is capable of linking and unlinking with others and each node can have
  children which are, themselves, nodes capable of linking to others and having
  children.

  As presented here, this class provides the basic functionality for more
  sophisticated lists.  The class does not define a storage mechanism for
  data besides that which is necessary to maintain a list of node; that is the
  purview of descendant classes.
}
type ANode = class(APrintingObject, CanStream)
  protected
    { Refers to the parent node of this node; @nil if the node is top-level}
    MyParent: ANode;
    { Refers to the previous sibling of this node; @nil if the node is the
      first node in its associated list.
    }
    MyPrevious: ANode;
    { Refers to the next sibling of this node; @nil if the node is the last
      node in its associated list.
    }
    MyNext: ANode;
    { Refers to the youngest child of this node; @nil if the node has no
      children.
    }
    MyChild: ANode;
    { Store the number of immediate children associated with this node.  This
      count does not include grandchildren.
    }
    myChildCount: TCensus;

  public
    { Construct a new node that will become the older sibling of the node
      specified by @code(Other).
    }
    constructor before(const Other: ANode); virtual;

    { Construct a new node that will become the younger sibling of the node
      specified by @code(Other).
    }
    constructor after(const Other: ANode); virtual;

    { Construct a new node that will become the youngest child of the node
      specified by @code(Other).
    }
    constructor asChildOf(const Other: ANode); virtual;

    { Initializer }
    function init: boolean; override;

    { Destroy the node, its children, and its older siblings.

      This method is called automatically when @code(TObject.free) is called
      on the node and so need not be called directly.

      Note that @bold(this method destroys all children of the node AS WELL AS
      the older siblings of the node) (by recursively traversing
      @link(ANode.Previous) and freeing each node encountered until a @nil
      result is found).  If this is not the desired behavior, the node should be
      detached from its siblings by calling @link(ANode.detach), and its
      children reassigned by calling @link(ANode.adoptChildrenOf) on another
      node, before this node is freed.

      Otherwise, @code(TObject.free) can be called on the youngest node in a
      list of nodes and it will cause the entire list to be freed.
    }
    destructor destroy; override;

    { Attaches the node specified by @code(Other) as a child of this node.
      @code(Other) becomes the youngest child of the node.
    }
    procedure attach(const Other: ANode); virtual;

    { Attaches this node to the one specified by @code(Other) as a child of that
      node.  This node becomes the youngest child of @code(Other).
    }
    procedure attachTo(const Other: ANode); virtual;

    { Inserts this node before the node specified by @code(Other).  This node
      becomes the immediate older sibling of @code(Other).  All other older
      siblings of @code(Other) are "promoted" first.
    }
    procedure insertBefore(const Other: ANode); virtual;

    { Inserts this node after the node specified by @code(Other).  This node
      becomes the immediate younger sibling of @code(Other).  All other younger
      siblings of @code(Other) are "demoted" first.
    }
    procedure insertAfter(const Other: ANode); virtual;

    { Appends the node specified by @code(Sibling) as the immediate younger
      sibling of this node.  All other younger siblings of this node are
      "demoted" first.
    }
    procedure append(const Sibling: ANode); virtual;

    { Prepends the node specified by @code(Sibling) as the immediate older
      sibling of this node.  All other older siblings of this node are
      "promoted" first.
    }
    procedure prepend(const Sibling: ANode); virtual;

    { Swaps the position of the current node for that of @code(Other).  Children
      of each node remain with their parents.
    }
    procedure swapWith(const Other: ANode); virtual;

    { Retrieve a reference to the sibling of the current node which is at the
      specified position relative to the current node.

      @code(relativePosition) may be a positive or negative value.  If a
      positive value greater than zero (@code(0)), then this method will
      recursively step through the chain of nodes attached to
      @link(ANode.Next Self.Next) until it reaches the node which is
      @code(relativePosition) steps down the chain.  If @code(relativePosition)
      is a negative value, then this method will recursively step through the
      chain of nodes attached to @link(ANode.Previous Self.Previous) until it
      reaches the node which is @code(relativePosition) steps up the chain.  In
      either case, if the end of the chain is reached before the number of
      steps specified by @code(relativePosition), then this method will return
      @nil.

      If @code(relativePosition) is zero (@code(0)), this method returns a
      reference to the current node (@code(Self)).

      It may be helpful to remember that the node specified by
      @link(ANode.Previous Self.Previous) will always be at position
      @code(-1) relative to @code(Self), while the node specified by
      @link(ANode.Next Self.Next) will always be at position @code(1) relative
      to @code(Self).
    }
    function SiblingAtRelativePosition(relativePosition: TNodeRelativeIndex): ANode;
      virtual;

    { Retrieve a reference to the sibling of the current node which is at the
      specified absolute position relative to the current node.

      This method follows the chain of nodes attached to
      @code(ANode.Previous Self.Previous) until it reaches the first node in
      the chain; it then steps back down through the chain until it reaches
      the node at the specified position.  If the end of the chain is reached
      before the number of steps specified by @code(absolutePosition), then
      this method will return @nil.

      This method operates on the assumption that node indices are zero-based;
      therefore, if @code(absolutePosition) is zero (@code(0)), this method
      will return a reference to the first node in the chain.

      This method can be used to find the node at a specified index within a
      list of nodes; however, it is worth noting that the index of a node
      within a linked list can be changed by adding or removing nodes in front
      of the node in question.  Furthermore, for large lists of nodes, this
      method can be inefficient as it must traverse the chain backwards and
      then forward again.
    }
    function SiblingAtAbsolutePosition(absolutePosition: TNodeAbsoluteIndex): ANode;
      virtual;

    { Detaches this node and its children from any siblings.  This effectively
      removes the node from its associated list, but it does not free the
      node or its children.
    }
    procedure detach; virtual;

    { Detaches this node from its parent.  This effectively removes the node
      from the list of nodes managed by the parent, but it does not free the
      node or its children.

      If the node has no parent, this routine does nothing.
    }
    procedure detachFromParent; virtual;

    { Adopts all children associated with the node specified by @code(Other),
      effectively transferring those children to the control of this node.
      @code(Other) will no longer have children after this method is called.

      Although the children adopted become the youngest children of this node,
      the relationships between the adoptees remain the same; in other words,
      the youngest child of @code(Other) will still be the youngest child.
    }
    procedure adoptChildrenOf(const Other: ANode); virtual;

    { Frees all children associated with the node.  Calling this method will
      recursively free the children of the node, and their children, and so
      forth.
    }
    procedure freeChildren; virtual;

    { Calculate the number of bytes that will be read from or written to a
      stream when the node, and just the node, is read or written.

      Descendant classes should first call this method, then add to the value
      returned.
    }
    function selfStreamingLength: TStreamIOSize; virtual;

    { Write the node, and just the node, to the specified stream.

      This method is called automatically by @link(ANode.writeTo).  It is
      designed to allow descendant classes to specify the data that each node
      should write to the given the stream without the need to worry about
      recursing through child nodes or siblings (@link(ANode.writeTo) takes
      care of that).

      The base implementation of this method simply writes the value of
      @link(ANode.census).  It is likely that descendant classes will override
      this method so that custom data associated with the node is also written
      to the stream.

      Note that it is important that any custom data associated with the node is
      written BEFORE the value of @link(ANode.census).  What this means is that
      you can override this method to write the custom data associated with your
      node class, then call the inherited routine to write the value of
      @link(ANode.census).  This will make it possible to recover the node and
      its children from the stream at a later point in time.  See
      @link(ANode.readFrom) for more details.

      @return(The total number of bytes written to @code(Dest).)
    }
    function writeSelfTo(const Dest: AStream): TStreamIOSize; virtual;

    { Print the node, and just the node, to the specified stream.

      This method is called automatically by @link(ANode.printTo).  It is
      designed to allow descendant classes to specify the data that each node
      should print ot the given stream without the need to worry about
      recursing through child nodes or siblings (@link(ANode.printTo) takes
      care of that).

      In the base implementation, this method simply prints the value of a
      call to @link(ANode.toString).

      If @code(prefix) and @code(suffix) are provided, they are printed before
      and after the representation of the node.

      @return(The total number of bytes printed to @code(Dest).)
    }
    function printSelfTo(const Dest: ATextOutputStream;
      prefix: AnsiString = '';
      suffix: AnsiString = ''): TStreamIOSize; virtual;

    // CanStream implementation ------------------------------------------------
    { Calculate the total number of bytes that will be read or written when
      either @link(ANode.readFrom) or @link(ANode.writeTo) is called.

      This method will first call itself on the immediate older sibling of the
      node, which will do the same until the first node is encountered.  That
      node will calculate the value for itself and its children, and then the
      recursion will proceed back down the list to the current node, ensuring
      that each node and its children are included in the final calculation.

      This method does not actually calculate the size of the the node itself;
      instead, it calls @link(ANode.selfStreamingLength), which is responsible
      for calculating the streaming size of the current node.

      This method can be called on the youngest node in a list of nodes and it
      will return with the total number of bytes required to stream the entire
      list.
    }
    function streamingLength: TStreamIOSize; virtual;

    { Write the node, its children, and its older siblings to the specified
      stream.

      This method will first call itself on the immediate older sibling of the
      node, which will do the same until the first node is encountered.  That
      node will write itself and its children, and then the recursion will
      proceed back down the list to the current node, ensuring that each node
      and its children are written to @code(Dest).

      This method does not actually write the node data itself; instead, it
      calls @link(ANode.writeSelfTo), which is responsible for writing the data
      retained by this node.

      This method can be called on the youngest node in a list of nodes and
      it will cause the entire list to be written to @code(Dest).

      @return(The total number of bytes written to @code(Dest).)
    }
    function writeTo(const Dest: AStream): TStreamIOSize; virtual;

    { Read the node and its children from the specified stream.

      This method will likely be overridden by descendant classes.  In its
      base implementation within @link(ANode), it simply reads the value of
      @link(ANode.census) from @code(Source), constructs a new instance of
      @link(ANode), and then recursively calls itself on that instance.

      Note that the new instance of @link(ANode) created will actually be an
      instance of that descendant of ANode upon which this method is called;
      the base implementation of this method obtains the current class by
      calling @code(TObject.classType) on itself and then calls on that class
      implementation to construct a new node.  What this means is that you can
      override the implementation of this method to read any custom data that
      is stored in @code(Dest), then call the inherited routine to ensure that
      all children of the node are likewise read, so long as any custom data
      related to the given node was written BEFORE the value of
      @link(ANode.census).

      @return(The total number of bytes read from @code(Dest).)
    }
    function readFrom(const Source: AStream): TStreamIOSize; virtual;

    // CanPrint implementation -------------------------------------------------
    { Obtain a string representation of the node, suitable for printing to a
      console or text-based device.

      The base implementation of this method within @link(ANode) simply returns
      the class name of the node and a count of the node's children.  The
      format of this string is specified by
      @link(nodeStringRepresentationPlural) or
      @link(nodeStringRepresentationSingular), respectively.
    }
    function toString: AnsiString; override;

    { Print a string representation of the node, its children, and its older
      siblings to the specified stream.

      This method will first call itself on the immediate older sibling of the
      node, which will do the same until the first node is encountered.  That
      node will print itself and its children, and then the recursion will
      proceed back down the list to the current node, ensuring that each node
      and its children are printed to @code(Dest).

      This method does not actually print the node data itself; instead, it
      calls @link(ANode.printSelfTo), which is responsible for writing the data
      retained by this node.

      @code(prefix) and @code(suffix) are not used directly by this method, but
      they are passed to @link(ANode.printSelfTo).

      This method can be called on the youngest node in a list of nodes and
      it will cause the entire list to be printed to @code(Dest).

      @return(The total number of bytes printed to @code(Dest).)
    }
    function printTo(const Dest: ATextOutputStream;
      prefix: AnsiString = '';
      suffix: AnsiString = ''): TStreamIOSize; override;

  // Properties ----------------------------------------------------------------
    { Determine whether or not the node has children.

      This method tests to see whether @link(ANode.Child) is not @nil and
      whether @link(ANode.census) is greater than zero.  If neither condition
      is true, then the routine returns @false to indicate that the node has
      no children; if both conditions are true, then the routine returns
      @true.
    }
    function hasChildren: boolean; virtual;

    { Retrieves a pointer to the parent of the node.  If the node is a top-level
      node, the value returned by this routine will be @nil.
    }
    function Parent: ANode; virtual;

    { Retrieves a pointer to the immediate older sibling of this node.  If
      this node is the first node in its associated list, then the value
      returned by this routine will be @nil.
    }
    function Previous: ANode; virtual;

    { Retrieves a pointer to the immediate younger sibling of this node.  If
      this node is the last node in its associated list, then the value returned
      by this routine will be @nil.
    }
    function Next: ANode; virtual;

    { Retrieves a pointer to the youngest immediate child of this node.  If
      the node has no children, then the value returned by this routine will be
      @nil.
    }
    function Child: ANode; virtual;

    { Retrieves the number of immediate children associated with this node.
      This count does not include grandchildren.
    }
    function census: TCensus; virtual;
  end;

{ This declaration simplifies the implementation of @link(ABinaryTree),
  @link(ALinkedList) and their descendants, which all contain and operate on
  descendants of @link(ANode).
}
type ANodeClass = class of ANode;
{$ENDIF}
{$IFDEF ReadResourceStrings}
  { Specifies the format to use when creating a string representation of
    @link(ANode) that has only one immediate child.  This format string is used
    by @link(ANode.toString).

    The string placeholder will be filled with the name of the class as
    returned by a call to @link(APrintingObject.displayName).  The integer
    placeholder will be filled with the number of immediate children of the
    node, which will always be one (1) when this format string is used.
  }
  nodeStringRepresentationSingular = '%s with %d child';

  { Specifies the format to use when creating a string representation of
    @link(ANode) that does not have one immediate child (it has either none or
    more than one child).  This format string is used by @link(ANode.toString).

    The string placeholder will be filled with the name of the class as
    returned by a call to @link(APrintingObject.displayName).  The integer
    placeholder will be filled with the number of immediate children of the
    node, as returned by a call to @link(ANode.census).
  }
  nodeStringRepresentationPlural = '%s with %d children';
{$ENDIF}
{$IFDEF ReadImplementation}
// ANode ***********************************************************************
// Construct a new node and insert it before the specified node
constructor ANode.before(const Other: ANode);

begin
  inherited new;

  if Other <> nil then
    Self.insertBefore(Other);
end;

// Construct a new node and insert it after the specified node
constructor ANode.after(const Other: ANode);

begin
  inherited new;

  if Other <> nil then
    Self.insertAfter(Other);
end;

{* Constructs a new instance of ANode and attaches to \p Other as a child.  If
   \p Other is #nil, this routine is functinally equivalent to ANode.new.

   \return A new instance of ANode on success; #nil on failure.
*}
constructor ANode.asChildOf(const Other: ANode);

begin
  inherited new;

  if Other <> nil then
    Other.attach(Self);
end;

{* Initializes a new instance of ANode.  This routine sets ANode.Parent,
   ANode.Previous, ANode.Next, and ANode.Child to #nil; it also sets
   ANode.census to zero (0).  The inherited routine is then called; in the
   default implementation, therefore, this routine will always return #true.

   \return #true if initialization was successful; #false if not.
*}
function ANode.init: boolean;

begin
  // Initialize our values
  MyParent := nil;
  // No children
  MyChild := nil;
  myChildCount := 0;
  // No siblings
  MyPrevious := nil;
  MyNext := nil;
  result := inherited init;
end;

{* Destroys the node and any nodes that precede it.  Children of the node are
   also freed.  This routine is designed to allow an entire list to be freed
   by calling the destructor of the node most recently added to that list.
*}
destructor ANode.destroy;

begin
  // Detach from our parent
  Self.detachFromParent;

  // Free our older siblings
  if MyPrevious <> nil then
    MyPrevious.free;

  // Free our children, if we have any
  if MyChild <> nil then
    MyChild.free;

  inherited destroy;
end;

{* Attaches \p Other to the list of child nodes managed by the node.  \p Other
   becomes the "youngest" child of the node, and a call to its destructor will
   free all children managed by the node.  After this call, ANode.Child will
   refer to \p Other.  If \p Other belonged to another list before this call
   was made, it will be removed from that list.
*}
procedure ANode.attach(const Other: ANode);

begin
  if Other <> nil then
  begin
    Other.detach;

    // Attach the child node
    Other.MyParent := Self;
    Other.MyPrevious := MyChild;

    if MyChild <> nil then
      MyChild.MyNext := Other;

    MyChild := Other;
    inc(myChildCount);
  end;
end;

{* Attaches the node to \p Other as one of the child nodes managed by that node.
   The node becomes the "youngest" child of \p Other, and a call to the node's
   destructor will free all children managed by \p Other.  If the node belonged
   to another list before this call was made, it will be removed from that list.
*}
procedure ANode.attachTo(const Other: ANode);

begin
  // Detach ourselves from any previous associations
  Self.detach;

  // Attach to our parent
  MyParent := Other;

  // We might have siblings...
  if MyParent <> nil then
  begin
    MyPrevious := MyParent.MyChild;
    if MyPrevious <> nil then
      MyPrevious.MyNext := Self;
    MyParent.MyChild := Self;
    inc(MyParent.myChildCount);
  end;
end;

{* Inserts the node into the same list as \p Other, as an "older sibling" of
   that node.  The node will inherit the same parent as \p Other.  If the
   node belonged to another list of nodes before this call was made, it will
   be removed from that list.
*}
procedure ANode.insertBefore(const Other: ANode);

begin
  if Other <> nil then
  begin
    // Detach ourselves from any previous associations
    Self.detach;

    // Attach to our parent
    MyParent := Other.MyParent;
    if MyParent <> nil then
      inc(MyParent.MyChildCount);

    // We have at least one sibling
    MyNext := Other;
    MyPrevious := Other.MyPrevious;
    Other.MyPrevious := Self;

    // See if we have another sibling and attach to it
    if MyPrevious <> nil then
      MyPrevious.MyNext := Self;
  end;
end;

{* Inserts the node into the same list as \p Other, as a "younger sibling" of
   that node.  The node will inherit the same parent as \p Other.  If the
   node belonged to another list of nodes before this call was made, it will
   be removed from that list.
*}
procedure ANode.insertAfter(const Other: ANode);

begin
  if Other <> nil then
  begin
    // Detach ourselves from any previous associations
    Self.detach;

    // Attach to our parent
    MyParent := Other.MyParent;
    if MyParent <> nil then
      inc(MyParent.myChildCount);

    // We have at least one sibling
    MyPrevious := Other;
    MyNext := Other.MyNext;
    Other.MyNext := Self;

    // See if we have another sibling and attach to it
    if MyNext <> nil then
      MyNext.MyPrevious := Self;
  end;
end;

{* Inserts \p Sibling into the same list of nodes as the one to which this
   node belongs.  \p Sibling becomes a "younger sibling" of this node and
   inherits the same parent.  If \p Sibling belonged to another list of nodes
   before this call was made, it will be removed from that list.
*}
procedure ANode.append(const Sibling: ANode);

begin
  if Sibling <> nil then
  begin
    Sibling.detach;

    // Attach the new node to our common parent
    Sibling.MyParent := MyParent;
    if MyParent <> nil then
      inc(MyParent.myChildCount);

    // If we already have a sibling after us, bump it out of the way
    if MyNext <> nil then
    begin
      Sibling.MyNext := MyNext;
      MyNext.MyPrevious := Sibling;
    end;

    MyNext := Sibling;
    Sibling.MyPrevious := Self;
  end;
end;

{* Inserts \p Sibling into the same list of nodes as the one to which this
   node belongs.  \p Sibling becomes an "older sibling" of this node and
   inherits the same parent.  If \p Sibling belonged to another list of nodes
   before this call was made, it will be removed from that list.
*}
procedure ANode.prepend(const Sibling: ANode);

begin
  if Sibling <> nil then
  begin
    Sibling.detach;

    // Attach the new node to our common parent
    Sibling.MyParent := MyParent;
    if MyParent <> nil then
      inc(MyParent.MyChildCount);

    // If we already have a sibling before us, bump it out of the way
    if MyPrevious <> nil then
    begin
      Sibling.MyPrevious := MyPrevious;
      MyPrevious.MyNext := Sibling;
    end;

    MyPrevious := Sibling;
    Sibling.MyNext := Self;
  end;
end;

{ Swap nodes }
procedure ANode.swapWith(const Other: ANode);

var
  { Stores our original previous and next references }
  OriginalPrevious, OriginalNext: ANode;

begin
  if Other = nil then
    exit;

  OriginalPrevious := MyPrevious;
  OriginalNext := MyNext;

  MyPrevious := Other.MyPrevious;
  MyNext := Other.MyNext;

  Other.MyPrevious := OriginalPrevious;
  Other.MyNext := OriginalNext;
end;

// Find the sibling at the specified relative position
function ANode.SiblingAtRelativePosition(relativePosition: TNodeRelativeIndex): ANode;

begin
  Result := Self;

  if relativePosition < 0 then
    while relativePosition < 0 do
    begin
      Result := Result.Previous;
      inc(relativePosition);

      if Result = nil then
        break;
    end

  else if relativePosition > 0 then
    while relativePosition > 0 do
    begin
      Result := Result.Next;
      dec(relativePosition);

      if Result = nil then
        break;
    end;
end;

// Find the sibling at the specified absolute position
function ANode.SiblingAtAbsolutePosition(absolutePosition: TNodeAbsoluteIndex): ANode;

begin
  // Find the beginning of the list
  Result := Self;

  while Result.Previous <> nil do
    Result := Result.Previous;

  // Step through the list
  while absolutePosition > 0 do
  begin
    Result := Result.Next;
    dec(absolutePosition);

    if Result = nil then
      break;
  end;
end;

{* Detaches the node and its children from the list of nodes being managed by
   its parent.  The parent list is updated to fill the gap left by the
   removal of this node.
*}
procedure ANode.detach;

begin
  // Detach from our siblings, if we have any
  if MyPrevious <> nil then
    MyPrevious.MyNext := MyNext;

  if MyNext <> nil then
    MyNext.MyPrevious := MyPrevious;

  // Detach from our parent, if we have one
  Self.detachFromParent;

  MyPrevious := nil;
  MyNext := nil;
  MyParent := nil;
end;

// Detach from the parent
procedure ANode.detachFromParent;

begin
  if MyParent <> nil then begin
    if MyNext <> nil then
      MyParent.MyChild := MyNext

    else
      MyParent.MyChild := MyPrevious;

    dec(MyParent.myChildCount);
  end;
end;

// Adopt all children of the specified node
procedure ANode.adoptChildrenOf(const Other: ANode);

var
  // Refers to a child node
  ThisChild, OlderChild: ANode;

begin
  if (Other <> nil) and (Other.hasChildren) then
  begin
    { Get the youngest child of the other node }
    ThisChild := Other.Child;
    OlderChild := ThisChild.Previous;

    { Find the oldest child of the other node }
    while OlderChild <> nil do
    begin
      ThisChild := OlderChild;
      OlderChild := ThisChild.Previous;
    end;

    { It becomes OUR youngest child, and all of its younger siblings as well }
    Self.attach(ThisChild);
    inc(myChildCount, Other.census);

    { Remove the reference from the other node }
    Other.MyChild := nil;
    Other.myChildCount := 0;
  end;
end;

{* Recursively frees all children of the node by calling ANode.free on the
   "youngest child" of the node.  ANode.Child is subsequently set to #nil,
   and ANode.census is set to zero (0).  If this node has no children, then
   this routine does nothing.
*}
procedure ANode.freeChildren;

begin
  // Free our children, if we have any
  if MyChild <> nil then
  begin
    MyChild.free;
    MyChild := nil;
    myChildCount := 0;
  end;
end;

// Calculate the number of bytes required to stream just the current node
function ANode.selfStreamingLength: TStreamIOSize;

begin
  result := sizeof(myChildCount);
end;

// Stream the node, and just the node, to the specified stream
function ANode.writeSelfTo(const Dest: AStream): TStreamIOSize;

begin
  result := Dest.write(sizeof(myChildCount), myChildCount);
end;

// Print the node, and just the node, to the specified stream
function ANode.printSelfTo(const Dest: ATextOutputStream;
  prefix: AnsiString = '';
  suffix: AnsiString = ''): TStreamIOSize;

begin
  result := Dest.printEach([prefix, Self.toString, suffix]);
end;

// CanStream implementation ----------------------------------------------------
// Calculate the streaming length of the node, its children, and older siblings
function ANode.streamingLength: TStreamIOSize;

begin
  result := 0;

  // Calculate older siblings first
  if MyPrevious <> nil then
    inc(result, MyPrevious.streamingLength);

  // Calculate ourselves
  inc(result, Self.selfStreamingLength);

  // Calculate our children
  if MyChild <> nil then
    inc(result, MyChild.streamingLength);
end;

// Write the node, its children, and its older siblings to the specified stream
function ANode.writeTo(const Dest: AStream): TStreamIOSize;

begin
  result := 0;

  if Dest <> nil then
  begin
    // Write our older siblings first
    if MyPrevious <> nil then
      result := MyPrevious.writeTo(Dest);

    // Write ourselves
    result := result + Self.writeSelfTo(Dest);
    // Write our children
    if MyChild <> nil then
      result := result + MyChild.writeTo(Dest);
  end;
end;

// Read the node and its children from the specified stream
function ANode.readFrom(const Source: AStream): TStreamIOSize;

{ Used to ensure the correct instance of ANode is recreated from the stream }
type
  ANodeClass = class of ANode;

var
  { Refers to the type of a new node }
  NewNodeClass: ANodeClass;
  // Refers to a new node
  NewNode: ANode;
  // Stores the number of children of the node
  nodeChildren: TCensus;

begin
  // First, read the number of children of the node
  nodeChildren := 0;
  result := Source.read(sizeof(nodeChildren), nodeChildren);

  NewNodeClass := ANodeClass(Self.classType);

  // Read each child
  while nodeChildren > 0 do
  begin
    { Construct the new node }
    NewNode := NewNodeClass.new;
    { Read the new node }
    result := result + NewNode.readFrom(Source);
    { Attach the new node as a child of this node }
    Self.attach(NewNode);

    dec(nodeChildren);
  end;
end;

// CanPrint implementation -----------------------------------------------------
{* Constructs a string representation of the node, suitable for output to a
   text-based stream.  The representation includes the class name of the node
   and the number of children it has.  These values are inserted into the
   string in accordance with the format specified by #nStringRepresentation.

   \return A string representation of the node, as described above.
*}
function ANode.toString: AnsiString;

begin

  if myChildCount <> 1 then
    result := format(nodeStringRepresentationPlural, [
      Self.displayName, myChildCount
    ])

  else
    result := format(nodeStringRepresentationSingular, [
      Self.displayName, myChildCount
    ]);
end;

{* Prints a string representation of the node to \p Dest.  The string
   representation used is that provided by a call to ANode.toString.

   \return The total number of bytes written to \p Dest.  This value will be
   zero (0) if \p Dest is #nil.
*}
function ANode.printTo(const Dest: ATextOutputStream;
  prefix: AnsiString = '';
  suffix: AnsiString = ''): TStreamIOSize;

begin
  result := 0;
  if Dest <> nil then
  begin
    // Print our older siblings first
    if MyPrevious <> nil then
      result := MyPrevious.printTo(Dest, prefix, suffix);

    // Print ourselves
    result := result + Self.printSelfTo(Dest, prefix, suffix);

    // Print each child node
    if MyChild <> nil then
      result := result + MyChild.printTo(Dest, prefix, suffix);
  end;
end;

// Properties ------------------------------------------------------------------
{* Determines whether or not the node has children.  This routine tests whether
   ANode.Child is #nil and whether ANode.census is zero (0).  If either
   condition occurs, this routine returns #false.  Otherwise, it returns
   #true.

   \return #true if the node has children; #false if not.
*}
function ANode.hasChildren: boolean;

begin
  result := (MyChild <> nil) and (myChildCount > 0);
end;

/// Retrieves a pointer to this node's parent
function ANode.Parent: ANode;

begin
  Result := MyParent;
end;

/// Retrieves a pointer to this node's previous sibling
function ANode.Previous: ANode;

begin
  Result := MyPrevious;
end;

/// Retrieves a pointer to this node's next sibling
function ANode.Next: ANode;

begin
  Result := MyNext;
end;

/// Retrieves a pointer to this node's first child
function ANode.Child: ANode;

begin
  Result := MyChild;
end;

/// Retrieves the number of children this node has
function ANode.census: TCensus;

begin
  result := myChildCount;
end;
{$ENDIF}

