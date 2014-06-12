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
{ Instances of this class can be used to iterate through the nodes in
  @link(ABinaryTree) and its descendants, starting with the root node and
  proceeding toward @link(ABinaryTree.YoungestNode).

  Nodes may be added and removed from the tree while the iterator is in use;
  the changes will be reflected by the iterator.  The obvious exception to
  this rule is that the node represented by @link(ABinaryTreeIterator.Current)
  cannot be deleted until the iterator passes over it without causing program
  crashes.  Likewise, the node directly attached to
  @link(ABinaryTreeIterator.Current) cannot be deleted or removed while
  @link(ABinaryTreeIterator.next) is operating (say, from a different thread).
  This is because we do not use reference counting or any other garbage
  collection routine to prevent the node from being deleted while the iterator
  is processing it.  It is the responsibility of the programmer to know how
  program memory is being managed.
}
type ABinaryTreeIterator = class(AnIterator)
  protected
    { Refers to the current node being processed }
    MyCurrentNode: ABinaryLeaf;
    { Refers to the tree being processed }
    MyTree: ABinaryTree;

  public
    { Construct an instance of @link(ABinaryTreeIterator) that will iterate
      over the nodes in @code(ThisTree).

      This method is called automatically by @link(ABinaryTree.iterator), which
      is the preferred method for obtaining an instance of this class.
    }
    constructor over(const ThisTree: ABinaryTree); virtual;

    { Initializer }
    function init: boolean; override;

    { Determine whether or not there are more nodes over which to iterate.
      For more information on this routine, see @link(AnIterator.continues).
    }
    function continues: boolean; override;

    { Retrieve the next node from the tree being processed.

      If there are no more nodes (the iterator has reached the end of the
      tree), then this routine does nothing.
    }
    procedure next; override;

    { Construct a shallow copy of the other object.

      This method overrides the behavior inherited from
      @link(AnObject.shallowCopyFrom): it calls that method, then checks to see
      whether @code(Other) is an instance of @link(ABinaryTreeIterator).  If so,
      it copies the values of
      @unorderedList(
        @item(@link(ABinaryTreeIterator.Current) and)
        @item(@link(ABinaryTreeIterator.Tree))
      )
      from @code(Other) to @code(Self), overwriting the values in @code(Self).
    }
    function shallowCopyFrom(const Other: AnObject): boolean; override;

    // Properties --------------------------------------------------------------
    { Retrieve a reference to the most recent node retrieved from the tree.

      The reference returned by this routine can be cast to a descendant of
      @link(ABinaryLeaf) if appropriate.  The caller should know what type of
      node exists within the tree being processed.

      The reference returned by this routine should NOT be freed by the caller.
    }
    function Current: ABinaryLeaf; virtual;

    { Retrieve a reference to the tree being processed.

      The reference returned by this routine can be cast to a descendant of
      @link(ABinaryTree) if appropriate.  The caller should know what type of
      tree is being processed by the iterator.

      The reference returned by this routine should NOT be freed by the caller.
    }
    function Tree: ABinaryTree; virtual;
end;
{$ENDIF}
{$IFDEF ReadImplementation}
// ABinaryTreeIterator *********************************************************
// Constructor
constructor ABinaryTreeIterator.over(const ThisTree: ABinaryTree);

begin
  inherited new;

  MyTree := ThisTree;

  if MyTree <> nil then
    MyCurrentNode := MyTree.Root;
end;

// Initializer
function ABinaryTreeIterator.init: boolean;

begin
  result := inherited init;

  if result then
  begin
    MyTree := nil;
    MyCurrentNode := nil;
  end;
end;

// Determine if there are more nodes available for iteration
function ABinaryTreeIterator.continues: boolean;

begin
  result := MyCurrentNode <> nil;
end;

// Retrieve the next node from the tree
procedure ABinaryTreeIterator.next;

begin
  if MyCurrentNode <> nil then
    MyCurrentNode := ABinaryLeaf(MyCurrentNode.Next);
end;

// Construct a shallow copy of the given object
function ABinaryTreeIterator.shallowCopyFrom(const Other: AnObject): boolean;

begin
  // Call the inherited routine first
  result := inherited shallowCopyFrom(Other);

  // Check to see whether the other item is compatible with us
  if not(Other is ABinaryTreeIterator) then
    result := false;

  if result then
  begin
    MyCurrentNode := ABinaryTreeIterator(Other).Current;
    MyTree := ABinaryTreeIterator(Other).Tree;
  end;
end;

// Properties ------------------------------------------------------------------
// Retrieve the most recent node retrieved from the tree
function ABinaryTreeIterator.Current: ABinaryLeaf;

begin
  Result := MyCurrentNode;
end;

// Retrieve the tree being processed
function ABinaryTreeIterator.Tree: ABinaryTree;

begin
  Result := MyTree;
end;
{$ENDIF}

