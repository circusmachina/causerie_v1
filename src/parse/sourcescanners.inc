{ sourcescanners.inc: Definitions and classes required to scan source code
    streams for tokens.

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
{ This class represents a scanner this used to return tokens from a source code
  stream.

  Instances of @link(ASourceScanner) are designed to work on instances of
  @link(ATextInputStream) which contain raw source code to tokenize.
}
type ASourceScanner = class(AScanner)
  protected
    { Refers to the language used to tokenize the source }
    MyLanguage: AParsedLanguage;

  public
    { Construct a new instance of @link(ASourceScanner) that will iterate over
      the source in @code(ThisSource) using @code(ThisLanguage) to tokenize the
      source.

      This method differs from the constructor for instances of @link(AScanner)
      in that it expects the source stream to be an instance of
      @link(ATextInputStream) rather than an intermediate code stream, and it
      requires a language definition to use in order to tokenize the source.
    }
    constructor forSource(const ThisSource: ATextInputStream;
      const ThisLanguage: AParsedLanguage); reintroduce; virtual;

    { Initializer }
    function init: boolean; override;

    { Retrieve the next token from the source.

      This method overrides the behavior inherited from @link(AScanner): where
      that method constructs and returns instances of @link(AToken) exclusively,
      this method will construct and return instances of @link(ATokenFromSource)
      for the most part.

      This method always succeeds; however, if the end of the source has been
      reached, the token created will be an instance of
      @link(AStreamEndingToken).  If @link(AScanner.CurrentToken) already
      refers to such an instance, then this routine does nothing.
    }
    procedure next; override;

    { Peek at the next token from the source without advancing the scanner.

      This method reads a token from the source and returns it, then rewinds
      the source position so that the token will be read again.

      The caller should free the token reference returned by this routine when
      it is no longer needed.  The scanner does NOT free the reference.
    }
    function Peek: AToken; override;

    // Properties --------------------------------------------------------------
    { Retrieve a reference to the language definition used to tokenize the
      source.

      The caller should NOT free this reference without first freeing the
      scanner; the reference is required in order to properly tokenize the
      source (it is passed to @link(ATokenFromSource.using).)
    }
    function Language: AParsedLanguage;
end;
{$ENDIF}
{$IFDEF ReadImplementation}
// Constructor
constructor ASourceScanner.forSource(const ThisSource: ATextInputStream;
  const ThisLanguage: AParsedLanguage);

begin
  inherited forSource(ThisSource);

  MyLanguage := ThisLanguage;
end;

// Initializer
function ASourceScanner.init: boolean;

begin
  result := inherited init;

  if result then
  begin
    MyLanguage := nil;
  end;
end;

// Retrieve the next token from the source
procedure ASourceScanner.next;

begin
  // We cannot read from a nonexistent source
  if MySource = nil then
    exit;

  // Do nothing if we've reached the end of the source
  if (MyCurrentToken <> nil) and (MyCurrentToken.opcode = TOKCAT_EOS) then
    exit;

  // This loop will be executed at least once
  repeat
    // Construct the next token
    MyCurrentToken :=
      ATokenFromSource.readUsing(MyLanguage, ATextInputStream(MySource));

    // Special behaviors
    case MyCurrentToken.opcode of
      TOKCAT_SPACE: begin
        // Are we consolidating whitespace?
        if SCAN_CONSOLIDATE_WHITESPACE in myTokenBehaviors then
          ASpaceToken(MyCurrentToken).setConsolidation(true);
      end;

      TOKCAT_EOL: begin
        // Are we consolidating line endings?
        if SCAN_CONSOLIDATE_LINE_ENDINGS in myTokenBehaviors then
          ALineEndingTokenFromSource(MyCurrentToken).setConsolidation(true);
      end;
    end;

    // Read the token
    MyCurrentToken.readFrom(MySource);

    // Update the line count
    if MyCurrentToken.opcode = TOKCAT_EOL then
      inc(myLineNumber, ALineEndingTokenFromSource(MyCurrentToken).lineCount);

    // Are we discarding whitespace
    if (MyCurrentToken.opcode = TOKCAT_SPACE) and
      (SCAN_NO_WHITESPACE in myTokenBehaviors) then
    begin
      MyCurrentToken.free;
      MyCurrentToken := nil;
    end

    else
      // All other tokens: one at a time
      break;

  until false;
end;

// Peek at the next token from the source
function ASourceScanner.Peek: AToken;

begin
  Result := nil;

  if MySource = nil then
    exit;

  // Read the token
  Result := ATokenFromSource.readUsing(MyLanguage, ATextInputStream(MySource));

  // Put back the token
  Result.returnTo(MySource);
end;

// Properties ------------------------------------------------------------------
// Retrieve the language used to tokenize the source
function ASourceScanner.Language: AParsedLanguage;

begin
  Result := MyLanguage;
end;
{$ENDIF}
