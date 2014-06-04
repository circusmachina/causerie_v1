{ parsers.inc: Chains of tokens that can be used as a list or a stack.

  This file is part of parsing.pp, a library of basic and useful parser classes.

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
{ This class represents a list of tokens that can be used as a sequential list
  or a stack.  It is designed to be used by instances of @link(AParser) and its
  descendants, wherein tokens read from a source stream are pushed onto the list
  and then popped off (and freed) as the source is parsed.
}
type ATokenList = class(ALinkedList)
  public
    { Initializer }
    function init: boolean; override;

    { Push @code(ThisToken) onto the end of the list.

      After calling this routine, @link(TokenList.LastToken) will return
      a reference to @code(ThisToken), while a call to @link(TokenList.Pop)
      will remove @code(ThisToken) and return it to the caller.
    }
    procedure push(const ThisToken: AToken); virtual;

    { Pop the last token from the list.

      If there are no tokens on the list, this routine will return @nil.
      Otherwise, it returns the most recent token pushed onto the list by a
      call to @link(ATokenList.push).
    }
    function Pop: AToken; virtual;

    // Properties --------------------------------------------------------------
    { Retrieve the token at the specified index.

      This method causes a sequential search of the list to be performed; for
      very large lists, therefore, it may be inefficient.

      Index zero (0) represents the first token in the list, while the last
      token is always at index (@code(ATokenList.census - 1)).  If @code(index)
      specifies a value that is greater than the number of items in the list,
      then this method returns the last item in the list.
    }
    function TokenAt(const index: longint): AToken; virtual;

    { Retrieve a reference to the first token in the list. }
    function FirstToken: AToken; virtual;

    { Retrieve a reference to the last token in the list. }
    function LastToken: AToken; virtual;
end;
{$ENDIF}
{$IFDEF ReadImplementation}
// ATokenList ******************************************************************
// Initializer
function ATokenList.init: boolean;

begin
  result := inherited init;

  if result then
  begin
    MyItemType := AToken;
  end;
end;

// Push a token onto the list
procedure ATokenList.push(const ThisToken: AToken);

begin
  if ThisToken <> nil then
    Self.pushItem(ThisToken);
end;

// Pop the last token from the list
function ATokenList.Pop:  AToken;

begin
  Result := AToken(Self.popItem);
end;

// Properties ------------------------------------------------------------------
// Retrieve the token at the specified index
function ATokenList.TokenAt(const index: longint): AToken;

begin
  Result := AToken(Self.itemAt(index));
end;

// Retrieve a reference to the first token in the list
function ATokenList.FirstToken: AToken;

begin
  Result := AToken(Self.FirstItem);
end;

// Retrieve a reference to the last item in the list
function ATokenList.LastToken: AToken;

begin
  Result := AToken(Self.LastItem);
end;
{$ENDIF}

