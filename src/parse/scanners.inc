{ scanners.inc: Definitions and classes required to scan intermediate code
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
{ This type defines the ways in which instances of @link(AScanner) handle
  instances of certain tokens:
  @unorderedList(
    @item(@link(SCAN_NO_WHITESPACE): Discard whitespace tokens when they are
      encountered.  When this behavior is enabled, @link(AScanner.CurrentToken)
      will never refer to an instance of @link(ASpaceToken), and
      @link(AScanner.next) will read tokens from the source until one is
      encountered that is not determined to be whitespace.
    )
    @item(@link(SCAN_CONSOLIDATE_WHITESPACE): Consolidate consecutive instances
      of the same whitespace character into a single instance of
      @link(ASpaceToken).  When this behavior is enabled, consecutive instances
      of the same whitespace character in the source will be collected into
      one instance of @link(ASpaceToken); otherwise, multiple instances of
      @link(ASpaceToken) will be returned.

      Obviously, this flag has no effect if @link(SCAN_NO_WHITESPACE) is
      enabled.
    )
    @item(@link(SCAN_CONSOLIDATE_LINE_ENDINGS): Consolidate consecutive
      instances of the same line ending character into a single instance of
      @link(ALineEndingToken).  When this behavior is enabled, consecutive
      instances of the same line ending character in the source will be
      collected into one instance of @link(ALineEndingToken); otherwise,
      multiple instances of @link(ALineEndingToken) will be returned.
    )
  )
}
type TScannerTokenBehavior = (
  { Discard whitespace tokens }
  SCAN_NO_WHITESPACE,
  { Consolidate whitespace into a single token }
  SCAN_CONSOLIDATE_WHITESPACE,
  { Consolidate line endings into a single token }
  SCAN_CONSOLIDATE_LINE_ENDINGS
);

{ This type defines a set of one or more token behaviors.  For more information
  on the behaviors available, see @link(TScannerTokenBehavior).
}
type TScannerTokenBehaviors = set of TScannerTokenBehavior;

const
  { This constant defines the default handling flags used by instances of
    @link(AScanner) when whitespace and line ending tokens are encountered.

    The default behavior is to ignore whitespace and to consolidate it into a
    single token if it is not ignored.  To set the behavior of a scanner,
    call @link(AScanner.setTokenBehaviors).
  }
  scnrDefaultTokenBehaviors: TScannerTokenBehaviors = [
    SCAN_NO_WHITESPACE, SCAN_CONSOLIDATE_WHITESPACE
  ];

{ This class represents a scanner that is used to return tokens from a stream.

  The base class, @link(AScanner), is meant to be used with files that
  contained parsed intermediate code.  The first descendant of this class,
  @link(ASourceScanner), is designed to be used with files that contain raw
  source.

  Scanners are constructed as iterators that iterate over the tokens in an
  arbitrary stream.  Thus, they can be used by a parser within an iterator
  loop: @longcode(#
    type MyParser = ASourceParser
      public
        function parse: integer; override;
    end;

    // ...

    // Parse the source
    function MyParser.parse: integer;

    begin
      // MySource and MyScanner are both protected properties of
      // AParser and its descendants.  MyLanguage is a protected property of
      // ASourceParser and its descendants.

      // Assuming, then, that the above properties have been been properly set
      // up and initialized elsewhere before this method is called...

      result := -1;

      // We can't parse a nonexistent source
      if MySource = nil then
        exit;

      // Establish the scanner
      // MySource is an instance of ASourceInputStream; the 'iterator' method
      // actually constructs an instance of ASourceScanner for us.
      MyScanner := ASourceScanner(MySource.iterator);

      if MyScanner = nil then
        exit;

      // Zero syntax errors thus far
      result := 0;

      // Iterator loop
      while MyScanner.continues do
      begin
        case MyScanner.CurrentToken.opcode of:
          // Parser code here
        end;

        // Get the next token
        MyScanner.next;
      end;

      // Dispose of the scanner
      MyScanner.free;

      // Report the number of syntax errors (MyLog is a protected property of
      // AParser and its descendants)
      result := MyLog.Counter('errors').value;
    end;
  #)
}
type AScanner = class(AnIterator)
  protected
    { Refers to the stream being scanned for tokens. }
    MySource: AStream;
    { Stores the current line number in the source. }
    myLineNumber: longword;
    { Refers to the most recent token retrieved from the source stream }
    MyCurrentToken: AToken;
    { Flags that indicate how the scanner handles certain tokens }
    myTokenBehaviors: TScannerTokenBehaviors;

  public
    { Construct a scanner that will iterate over the given source stream.

      Scanning begins at the current position within @code(ThisSource).
    }
    constructor over(const ThisSource: AStream); virtual;

    { Initializer }
    function init: boolean; override;

    { Return the specified token to the source stream.

      This method can be used by parsers to "peek" at a token and then return
      it to the source for further processing.

      @code(ThisToken) is freed by this routine and should not be used after
      the routine returns.

      This method calls @link(AToken.returnTo).
    }
    procedure returnToken(const ThisToken: AToken); virtual;

    { Return the token most recently read from the source back to the source.

      This method can be used by parsers to "peek" at a token and then return
      it to the source for further processing.  The current token is freed by
      this routine and should not be used after the routine returns.

      This method calls @link(AToken.returnTo).
    }
    procedure rewind; virtual;

    { Retrieve the next token from the source.

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
    function Peek: AToken; virtual;

    { Determine whether or not there are more tokens to retrieve from the
      source.

      If the source indicates that its end has been reached, this method will
      return @false; otherwise, it will return @true, because the last token
      to retrieve from the source will always be an instance of
      @link(AStreamEndingToken).
    }
    function continues: boolean; override;

    // Properties --------------------------------------------------------------
    { Retrieve a reference to the source stream being scanned for tokens.

      This reference should NOT be freed before the scanner itself is freed,
      otherwise, undefined things (program crashes) will happen.
    }
    function Source: AStream;

    { Retrieve the current line number from the source.

      This function is primarily used by the parser to output error and
      status messages.  The line number is updated whenever an instance of
      @link(ALineEndingToken) is encountered by the scanner.
    }
    function lineNumber: longword;

    { Retrieve a reference to the most recent token read from the stream.

      The caller should free this reference.  This is NOT done by the scanner,
      not even when a new token is read from the source.  This behavior is by
      design, since @link(AParser) and its descendants organize tokens into
      instances of @link(ATokenList), which will free all of its token
      references when the list itself is destroyed.
    }
    function CurrentToken: AToken;

    { Retrieve the behaviors defined for the scanner when certain tokens are
      encountered.  For more information on the available behaviors, see
      @link(TScannerTokenBehavior).
    }
    function tokenBehaviors: TScannerTokenBehaviors; virtual;

    { Set the behaviors for the scanner when certain tokens are encountered.
      For more information on the available behaviors, see
      @link(TScannerTokenBehavior).

      @return(The previous value of @link(AScanner.tokenBehaviors).)
    }
    function setTokenBehaviors(const newBehaviors: TScannerTokenBehaviors): TScannerTokenBehaviors;
      virtual;
end;
{$ENDIF}
{$IFDEF ReadImplementation}
// Constructor
constructor AScanner.over(const ThisSource: AStream);

begin
  inherited new;

  MySource := ThisSource;

  // Load the first token automatically
  Self.next;
end;

// Initializer
function AScanner.init: boolean;

begin
  result := inherited init;

  if result then
  begin
    MySource := nil;
    myLineNumber := 1;
    MyCurrentToken := nil;
    myTokenBehaviors := scnrDefaultTokenBehaviors;
  end;
end;

// Return the specified token to the source
procedure AScanner.returnToken(const ThisToken: AToken);

begin
  if ThisToken <> nil then
  begin
    ThisToken.returnTo(MySource);
    ThisToken.free;
  end;
end;

// Return the current token to the source
procedure AScanner.rewind;

begin
  Self.returnToken(MyCurrentToken);
  MyCurrentToken := nil;
end;

// Retrieve the next token from the source
procedure AScanner.next;

var
  // Refers to the next token in the source
  NextToken: AToken;

begin
  // We cannot read from a nonexistent source
  if MySource = nil then
    exit;

  // Do nothing if we've reached the end of the source
  if (MyCurrentToken <> nil) and (MyCurrentToken.opcode = TOKCAT_EOS) then
    exit;

  // This loop will be executed at least once
  repeat
    NextToken := nil;

    // Construct and read the next token
    MyCurrentToken := AToken.from(MySource);
    MyCurrentToken.readFrom(MySource);

    // Special behaviors
    case MyCurrentToken.opcode of
      TOKCAT_SPACE: begin
        // Are we skipping whitespace?
        if SCAN_NO_WHITESPACE in myTokenBehaviors then
        begin
          MyCurrentToken.free;
          MyCurrentToken := nil;
        end

        // Are we consolidating whitespace?
        else if SCAN_CONSOLIDATE_WHITESPACE in myTokenBehaviors then
        begin
          // Check the next token
          NextToken := Self.Peek;

          if NextToken.opcode <> TOKCAT_SPACE then
          begin
            NextToken.free;
            break;
          end;
        end;
      end;

      TOKCAT_EOL: begin
        // Update line count
        inc(myLineNumber, ALineEndingToken(MyCurrentToken).lineCount);

        // Are we consolidating line endings?
        if SCAN_CONSOLIDATE_LINE_ENDINGS in myTokenBehaviors then
        begin
          // Check the next token
          NextToken := Self.Peek;

          if NextToken.opcode <> TOKCAT_EOL then
          begin
            NextToken.free;
            break;
          end;
        end;
      end;

    else
      // All other tokens: one at a time
      break;
    end;
  until false;
end;

// Peek at the next token from the source
function AScanner.Peek: AToken;

begin
  Result := nil;

  if MySource = nil then
    exit;

  // Read the token
  Result := AToken.from(MySource);

  // Put back the token
  Result.returnTo(mySource);
end;

// Determine whether or not there are more tokens to retrieve
function AScanner.continues: boolean;

begin
  result := (MyCurrentToken <> nil) and
    (MyCurrentToken.opcode <> TOKCAT_EOS);
end;

// Properties ------------------------------------------------------------------
// Retrieve a reference to the source stream
function AScanner.Source: AStream;

begin
  Result := MySource;
end;

// Retrieve the current line number in the source
function AScanner.lineNumber: longword;

begin
  result := myLineNumber;
end;

// Retrieve a reference to the most recent token read from the source
function AScanner.CurrentToken: AToken;

begin
  Result := MyCurrentToken;
end;

// Retrieve the list of special token handling behaviors
function AScanner.tokenBehaviors: TScannerTokenBehaviors;

begin
  result := myTokenBehaviors;
end;

// Set new token handling behaviors
function AScanner.setTokenBehaviors(const newBehaviors: TScannerTokenBehaviors): TScannerTokenBehaviors;

begin
  result := myTokenBehaviors;
  myTokenBehaviors := newBehaviors;
end;
{$ENDIF}

