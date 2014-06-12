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
{ This interface will be adopted by all classes that are able to produce string
  representations of themselves and their data, and which can also print those
  representations to an arbitrary stream.

  This interface is useful for producing debugging output.
}
type CanPrint = interface
  { Construct and return a string representation of the object and its data.
  }
  function toString: AnsiString;

  { Print a string representation of the class and its data to the specified
    stream.  If @code(prefix) or @code(suffix) are provided, these are printed
    before and after the string representation, respectively.

    @code(prefix) and @code(suffix) are defined to allow the caller to refine
    how the information is printed; for instance, the caller could specify
    @code(lineEnding) for @code(suffix) to cause a newline to be printed after
    the instance representation.  When called on a class that calls this method
    recursively, this will ensure that the printed output is still easy to read.

    Classes that incorporate or manage instances of other classes (such as
    @link(ABinaryTree)) may print not only their own representations, but those
    of the classes that they manage.

    @returns(The total number of bytes printed to @code(Dest).)
  }
  function printTo(const Dest: AStream;
    prefix: AnsiString = '';
    suffix: AnsiString = ''): TStreamIOSize;
end;

{ This class acts as a repository for some string functions that are used by
  various classes.  These routines will work for Unicode strings encoded with
  UTF-8 as well as ASCII-encoded strings.
}
type Charstring = class
    public
      { Retrieve the number of characters in the given string.

        This method differs from @code(System.length) in that it counts the
        number of UTF-8 characters in the string; @bold(NOT) the total number
        of bytes.  As of this writing, @code(System.length) assumes that an
        @code(AnsiString) has exactly one byte per character; since UTF-8
        strings may use several bytes per character, this is not an accurate
        way to determine the total number of characters in a string.

        Internally, this method calls @code(LazUTF8.utf8Length) and returns
        the result.
      }
      class function census(const thisString: string): integer;

      { Retrieve the character at the given character position within the
        string.

        This method should be used to obtain the character at the given
        character position within a string.  It is potentially incorrect to
        simply index the string, as you would do with a typical Pascal string.
        As of this writing, the compiler assumes that a typical
        @code(AnsiString) uses exactly one byte per character; thus, indexing
        the string will give you a @italic(byte) offset into the string -- but
        because strings encoded with UTF-8 may use more than one byte per
        character, this may not be the actual offset of a given character.

        Because UTF-8 strings may use more than one byte per character, this
        method returns a UTF-8 encoded string rather than a @code(char).  The
        string will contain the UTF-8 encoded character at the given
        @code(index).  If @code(index) specifies a positive value, it is taken
        to refer to the desired character position relative to the beginning
        of the string; in such cases, the first character is always located at
        index 1.  If @code(index) specifies a negative value, it is taken to
        refer to the desired character position relative to the end of the
        string; in such cases, the last character is always located at index
        -1.  If @code(index) specifies a value that is greater than the number
        of characters in @code(thisString), then this routine returns an empty
        string.

        Internally, this method calls @code(LazUTF8.utf8Copy).
      }
      class function character(const thisString: string;
        index: integer): string;

      { Determine the number of bytes occupied by the given character.

        This method simply calls @code(LazUtf8.utf8CharacterLength) and returns
        the result.  If @code(thisChar) is an empty string, then this method
        returns zero (0).  If @code(thisChar) contains more than one character,
        the remaining characters are ignored.
      }
      class function characterLength(const thisChar: string): integer;

      { Calculates the hash value of the given string.

        This routine is useful for deriving sort keys for instances of
        @link(ABinaryTree) that accept strings as keys.  It is therefore used
        heavily by @link(AStringTree), @link(ASymbolTable), and other classes.

        The method makes use of the @code(djb2) algorithm, originally written
        by Professor Daniel J. Bernstein and listed at
        http://www.cse.yorku.ca/~oz/hash.html.
      }
      class function hashValueOf(const thisString: string): int64;

      { Collects characters from the string until the specified delimiter is
        encountered.

        This routine helps to tokenize a string which contains a list of values
        that are separated by an arbitrary delimiter.  The method "bites off"
        each value from the string, using the value of @code(delimiter) to
        determine where to stop removing characters.  @code(s) is truncated
        from the left side, removing both the delimiter and the value.

        Successive calls to this routine using the same string will result
        in parsing every value from the string.

        @return(The next value removed from the string, or an empty string if
        there are no more values to remove.  An empty string is also returned
        if @code(s) or @code(delimiter) are empty strings.  If no instance of
        @code(delimiter) is found in @code(s), this routine will simply return
        @code(s).)
      }
      class function chomp(var s: string; const delimiter: string): string;

      { Return the quoted form of @code(s).

        This method adds @code(beginQuote) and @code(endQuote) to @code(s) and
        returns the result.  If either @code(beginQuote) or @code(endQuote)
        are empty strings, the values in @link(charsBeginQuoteCharacter) and
        @link(charsEndQuoteCharacter) are used.
      }
      class function quote(const s: string;
        beginQuote: string = '';
        endQuote: string = ''): string;

      { Converts @code(s) to uppercase.

        This method calls @code(LazUtf8.utf8LowerCase).  @code(language) should
        be specified using the two-character given in ISO 639-1 (e.g.,
        @code('en') for English, @code('tr') for Turkish, etc.), but it may
        also be omitted or left blank, in which case the locale is ignored.

        If @code(s) is an empty string, or if @code(s) has no lowercase
        characters, this routine will return @code(s) unchanged.
      }
      class function toUpperCase(const s: string;
        const language: string = ''): string;

      { Converts @code(s) to lowercase.

        This method calls @code(LazUtf8.utf8UpperCase).  @code(language) should
        be specified using the two-character given in ISO 639-1 (e.g.,
        @code('en') for English, @code('tr') for Turkish, etc.), but it may
        also be omitted or left blank, in which case the locale is ignored.

        If @code(s) is an empty string, or if @code(s) has no uppercase
        characters, this routine will return @code(s) unchanged.
      }
      class function toLowerCase(const s: string;
        const language: string = ''): string;

      { Find the first occurrence of @code(needle) in @code(haystack),
        optionally starting at the specified character offset.

        This method calls @code(LazUtf8.utf8Pos).  If @code(needle) is found,
        the character offset of its location within @code(haystack) is
        returned.  The first character of @code(haystack) will always be at
        position 1.

        If @code(needle) is not found, then this routine returns zero (0).
      }
      class function pos(const needle, haystack: string;
        startPos: integer = 1): integer;

      { Determines whether @code(s) ends with the string specified by
        @code(ending).

        This method checks to see whether the characters at the end of
        @code(s) all match those in @code(ending); if so, it returns @true;
        otherwise, it returns @false.  If @code(s) or @code(ending) are
        empty strings, this routine returns @false.
      }
      class function endsWith(const s: string; const ending: string): boolean;

      { Determines whether @code(s) ends with the string specified by
        @code(ending) and, if so, replaces it with @code(replacement).

        If @code(s) does not end with @code(ending), or if any of the strings
        passed are empty, this routine returns @code(s) unchanged.
      }
      class function replaceEnding(const s: string;
        const ending: string; const replacement: string): string;

      { Determine if @code(s) consists entirely of uppercase characters.

        This method might also be said to determine if @code(s) contains no
        lower case characters.  It compares every character in @code(s) against
        those specified by @link(charsLowercaseLetters).  If it finds a match,
        then the routine returns @false; otherwise, it returns @true.
      }
      class function isUppercase(const s: string): boolean;

      { Determine if @code(s) consists entirely of lowercase characters.

        This method might also be said to determine if @code(s) contains no
        upper case characters.  It compares every character in @code(s) against
        those specified by @link(charsUppercaseLetters).  If it finds a match,
        then the routine returns @false; otherwise, it returns @true.
      }
      class function isLowercase(const s: string): boolean;

      { Expands a string that is written in CamelCase, adding spaces where
        a capital letter follows a lowercase letter.

        This method processes each character in the string and determines
        whether that character is a capital letter by calling
        @link(CharString.isUppercase).  If the character is uppercase and
        follows a lower-case letter, then a space is added to the output.
      }
      class function expandCamelCase(const s: string): string;
end;
{$ENDIF}
{$IFDEF ReadResourceStrings}
  { The default opening quote character used }
  charsBeginQuoteCharacter = '"';
  { The default closing quote character used }
  charsEndQuoteCharacter = '"';
  { Letters that are considered uppercase }
  charsUppercaseLetters = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ';
  { Letters that are considered lowercase }
  charsLowercaseLetters = 'abcdefghijklmnopqrstuvwxyz';
{$ENDIF}
{$IFDEF ReadImplementation}
// Charstring ******************************************************************
// Determine the number of characters in a UTF-8 or ASCII string
class function Charstring.census(const thisString: string): integer;

begin
  result := LazUtf8.utf8Length(thisString);
end;

// Retrieve the character at the given position within a UTF-8 or ASCII string
class function Charstring.character(const thisString: string;
  index: integer): string;

begin
  result := '';

  // Deal with invalid indices
  if index > Charstring.census(thisString) then
    exit;

  // Deal with negative indices
  if index < 0 then
    inc(index, Charstring.census(thisString) + 2);

  // Convert to zero-based index instead of Pascal 1-based index
  dec(index);

  if index < 0 then
    index := 0;

  // Find the character and copy it
  result := LazUtf8.utf8Copy(thisString, index, 1);
end;

// Retrieve the size of the character at the given position within a string
class function Charstring.characterLength(const thisChar: string): integer;

begin
  result := 0;

  if thisChar = '' then
    exit;

  result := LazUtf8.utf8CharacterLength(@thisChar[1]);
end;

// Calculate the hash value of the given string
class function Charstring.hashValueOf(const thisString: string): int64;

var
  i: int64;
  ch: pchar;
  c: integer;

begin
  result := 0;

  if thisString <> '' then
  begin
    result := 5381;
    ch := @thisString[1];
    i := System.length(thisString);

    repeat
      c := ord(ch^);
      result := ((result shl 5) + result) + c;         // hash * 33 + c
      inc(ch);
      dec(i);
    until i < 1;
  end;
end;

// Return characters until delimiter is encountered
class function Charstring.chomp(var s: string; const delimiter: string): string;

var
  // Loop counters
  i, j: integer;
  // Accumulates characters from the delimiter
  thisDelimiter: string;
  // Stores the length of the list string
  listLength: integer;
  // Stores the length of the delimiter string
  delimLength: integer;

begin
  result := '';

  if (s <> '') and (delimiter <> '') then
  begin
    listLength := Charstring.census(s);
    delimLength := Charstring.census(delimiter);
    i := 1;

    while (i <= listLength) do
    begin
      thisDelimiter := '';

      // Collect characters from the string to see if we have a delimiter
      for j := 1 to delimLength do
      begin
        thisDelimiter := thisDelimiter + Charstring.character(s, i);
        inc(i);

        if i > listLength then
          break;
      end;

      // Is it the delimiter?
      if thisDelimiter <> delimiter then
        // Nope -- collect the characters and continue
        result := result + thisDelimiter

      else
        break;

      inc(i);
    end;

    // Chomp the item and the delimiter from the beginning of the string
    if result <> '' then
    begin
      if Charstring.census(result) < listLength then
        s := LazUtf8.utf8Copy(s, Charstring.census(result) + delimLength,
          listLength)

      else
        s := '';
    end;

  end;
end;

// Return the quoted form of the specified string
class function Charstring.quote(const s: string;
  beginQuote: string = ''; endQuote: string = ''): string;

begin
  if s <> '' then
  begin
    if beginQuote = '' then
      beginQuote := charsBeginQuoteCharacter;

    if endQuote = '' then
      endQuote := charsEndQuoteCharacter;

    result := beginQuote + s + endQuote;
  end

  else
    result := s;
end;

// Converts a string to uppercase
class function Charstring.toUpperCase(const s: string;
  const language: string = ''): string;

begin
  result := LazUtf8.utf8UpperCase(s, language);
end;

// Converts a string to lower case
class function Charstring.toLowerCase(const s: string;
  const language: string = ''): string;

begin
  result := LazUtf8.utf8LowerCase(s, language);
end;

// Find a substring within the given string
class function Charstring.pos(const needle, haystack: string;
  startPos: integer = 1): integer;

begin
  result := 0;

  if (needle = '') or (haystack = '') then
    exit;

  // Deal with invalid start offset
  if startPos > Charstring.census(haystack) then
    exit;

  // Deal with negative indices
  if startPos < 0 then
    inc(startPos, Charstring.census(haystack) + 1);

  if startPos < 0 then
    startPos := 0;

  // Search the string
  result := LazUtf8.utf8Pos(needle, haystack, startPos);
end;

// Determines whether the given string has the given ending
class function Charstring.endsWith(const s: string;
  const ending: string): boolean;

begin
  result := false;

  if (ending <> '') and (s <> '') then
    result :=
      (Self.pos(ending, s) = (Charstring.census(s) - Charstring.census(ending)));
end;

/// Replaces the specified ending with another
class function Charstring.replaceEnding(const s: string;
  const ending: string; const replacement: string): string;

begin
  result := s;

  if Charstring.endsWith(ending, s) then
  begin
    result := LazUtf8.utf8Copy(s, 1, (Charstring.census(s) - Charstring.census(ending)));
    result := result + replacement;
  end;
end;

// Determines if the entire string is uppercase
class function Charstring.isUppercase(const s: string): boolean;

var
  // Loop counter
  i: longword;

begin
  result := false;

  if System.length(s) = 0 then
    exit;

  for i := 1 to System.length(s) do
    if LazUtf8.utf8Pos(s[i], charsLowercaseLetters) > 0 then
      exit;

  result := true;
end;

// Determines if the entire string is lowercase
class function Charstring.isLowercase(const s: string): boolean;

var
  // Loop counter
  i: longword;

begin
  result := false;

  if Self.census(s) = 0 then
    exit;

  for i := 1 to Charstring.census(s) do
    if Self.pos(Self.character(s, i), charsUppercaseLetters) > 0 then
      exit;

  result := true;
end;

// Expand a camel-case string
class function Charstring.expandCamelCase(const s: string): string;

var
  // A loop counter
  i: integer;
  // The previous character examined
  previousChar: string;

begin
  result := '';
  previousChar := 'A';

  for i := 1 to Charstring.census(s) do
  begin
    // Insert a space into the output for every capital letter in the string
    if (LazUtf8.utf8pos(Charstring.character(s, i), charsUppercaseLetters) > 0)
      and (i > 1) then
    begin
      // ...but only if the previous character was lowercase
      if LazUtf8.utf8Pos(previousChar, charsUpperCaseLetters) <= 0 then
        result := result + ' ';
    end;

    result := result + Charstring.character(s, i);

    previousChar := Charstring.character(s, i);
  end;
end;
{$ENDIF}

