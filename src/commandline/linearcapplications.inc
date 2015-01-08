{ linearcstatements.inc - A descendant of AConsoleApplication that builds and
    executes a command-line parser automatically.

  This file is part of commandline.pp, a library for robust command-line
  parsing.

  This program is free software: you can redistribute it and/or modify it under
  the terms of the GNU General Public License as published by the Free Software
  Foundation; either version 3 of the License or, at your option, any later
  version.

  This program is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABLITY or FITNESS
  FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
  details.

  You should have received a copy of the GNU General Public License along with
  this program.  If not, see <http://www.gnu.org/licenses/>.
}
{$IFDEF ReadInterface}
{ This class represents a single process running as a console application,
  which expects to receive arguments on its command line and which can parse
  those arguments with an instance of @link(ALinearCParser).

  This class modifies the behavior of @link(AConsoleApplication) somewhat:
  the @link(ALinearCConsoleApplication.run run) method is overridden to call the
  @link(ALinearCConsoleApplication.defineOptions) before
  calling @link(ALinearCConsoleApplication.parseCommandLine).
}
type ALinearCConsoleApplication = class(AConsoleApplication)
  protected
    { Refers to the command line arguments and options accepted by the program }
    MyOptions: ACommandLineOptionDictionary;
    { Refers to the log used for error and diagnostic output by the program }
    MyLog: ALog;

    { Define the command line options accepted by the program.

      This method may be overridden by descendant classes.  In the base
      implementation of @link(ALinearCConsoleApplication), it defines two
      @link(ACommandLineFlag flag options): one for "help" and one for
      "version".
    }
    procedure defineOptions; virtual;

  public
    { Initializer}
    function init: boolean; override;

    { Destroy the application instance.

      This method frees @link(ALinearCConsoleApplication.Options), then calls
      the inherited routine.
    }
    destructor destroy; override;

    { Parse the command line.

      This method constructs an instance of @link(AStringStream) to represent
      the command line arguments passed to the program; it then passes that
      stream to an instance of @link(ALinearCParser), along with the items in
      @link(ALinearCConsoleApplication.Options), so that the command line can
      be parsed.

      @return(The value returned by @link(ALinearCParser.parse), which should
      indicate the number of errors that occurred while parsing.)
    }
    function parseCommandLine: int64; virtual;

    { Execute the application.

      This method builds on the behavior inherited from
      @link(AConsoleApplication.run): after calling the inherited routine, it
      calls @link(ALinearCConsoleApplication.defineOptions), then
      @link(ALinearCConsoleApplication.parseCommandLine).

      When the command-line arguments have been parsed, this method will check
      to see whether the "help" or "version" flags have been specified; if so,
      it calls @link(ALinearCConsoleApplication.printHelpTo) or
      @link(ALinearCConsoleApplication.printVersionTo).

      If either @link(ALinearCConsoleApplication.Options) or
      @link(ALinearCConsoleApplication.Log) are @nil when this method is
      entered, meaning that no descendant class has constructed a list of
      options or a log as part of their initialization, then this method will
      construct a basic instance of each.  The log will output to
      @link(AConsoleApplication.ErrorOut Self.ErrorOut).

      @return(Zero if execution was successful up to the point defined by this
      method (descendant classes may perform additional execution afterward),
      non-zero if an error occurred.)
    }
    function run: integer; override;

    { Print usage information for the program to the specified stream.

      This method is called automatically by
      @link(ALinearCConsoleApplication.run), after the command line options have
      been parsed, if the @code(--help) option was specified by the user.  It
      calls @link(ANode.printTo ACommandLineOptionsDictionary.printTo) on
      @link(ALinearCConsoleApplication.Options Self.Options).
    }
    procedure printHelpTo(const ThisStream: ATextOutputStream); virtual;

    { Print version information for the program to the specified stream.

      This method is called automatically by
      @link(ALinearCConsoleApplication.run), after the command line options have
      been parsed, if the @code(--version) option was specified by the user.

      The method uses the key/value pairs specified in
      @link(AnApplication.VersionInfo Self.VersionInfo) to print the name of
      the program and its version.  The format of this output is controlled by
      @link(lcappVersionStringFormat).

      Additional items can be specified in the
      @code(additionalItems) array.
    }
    procedure printVersionTo(const ThisStream: ATextOutputStream;
      const additionalItems: array of string); virtual;

    // Properties --------------------------------------------------------------
    { Retrieve a reference to the dictionary of command-line options accepted
      by the program.

      The caller should NOT attempt to free the reference returned by this
      method; that will be done with the application instance is freed.
    }
    function Options: ACommandLineOptionDictionary; virtual;

    { Retrieve a reference to the log used for error and diagnostic output by
      the program.

      The caller should NOT attempt to free the reference returned by this
      method; that will be done when the application instance is freed.
    }
    function Log: ALog; virtual;
end;

const
  { This array can be used to determine whether and how
    @link(ALinearCConsoleApplication.printVersionTo) prints additional items
    besides just the program name and version.

    Each string is printed to the stream specified.  If the strings represent
    the name of a key that may be in
    @link(AnApplication.VersionInfo Self.VersionInfo), and if such a key is
    found, then the value associated with that key is printed.  Otherwise, the
    literal value of the string is printed instead.

    If the string to be printed is not contained in
    @link(tosWrapDefaultBreakCharacters), then it is printed by means of
    @link(ATextOutputStream.printWrapped); otherwise it is printed as is, by
    means of @link(ATextOutputStream.print).
  }
  lcappVersionDefaultAdditionalItems: array[0..2] of string = (
    vinfLegalCopyright,             // printWrapped() will add an EOL after this
    vinfComments, System.lineEnding // printWrapped() will add an EOL after this
  );

{$ENDIF}
{$IFDEF ReadResourceStrings}
  lckwHelpShortString = '?';
  lckwHelpLongString = 'help';
  lckwHelpDescription = 'Display this usage information.';
  lckwVersionLongString = 'version';
  lckwVersionDescription = 'Display program version information.';
  lcappUsageStringFormat = 'Usage: %s [options] %s' + lineEnding;
  lcappVersionStringFormat = '%s %s' + lineEnding;
{$ENDIF}
{$IFDEF ReadImplementation}
// ALinearCConsoleApplication **************************************************
// Initializer
function ALinearCConsoleApplication.init: boolean;

begin
  result := inherited init;

  if result then
  begin
    MyOptions := nil;
  end;
end;

// Destructor
destructor ALinearCConsoleApplication.destroy;

begin
  // Free the list of command line options
  if MyOptions <> nil then
    MyOptions.free;

  // Free the log
  if MyLog <> nil then
    MyLog.free;

  inherited destroy;
end;

// Define the options accepted by the program
procedure ALinearCConsoleApplication.defineOptions;

begin
  if MyOptions = nil then
    exit;

  // Add the '--help' flag
  MyOptions.add(ACommandLineFlag.named(lckwHelpLongString, // name
    false,                                                 // is required?
    false,                                                 // is rest?
    false,                                                 // default value
    lckwHelpDescription),                                  // description
    [lckwHelpShortString]);                                // abbreviations

  // Add the '--version' flag
  MyOptions.add(ACommandLineFlag.named(lckwVersionLongString, // name
    false,                                                    // is required?
    false,                                                    // is rest?
    false,                                                    // default value
    lckwVersionDescription),                                  // description
    []);                                                      // abbreviations
end;

// Parse the command line
function ALinearCConsoleApplication.parseCommandLine: int64;

var
  // Refers to the string stream used as the source of options
  OptionsStream: ATextOutputStream;
  // Loop counter, used to collect options
  i: longword;
  // Refers to the parser used to parse the command line options
  CommandLine: ALinearCParser;

begin
  result := -1;

  if (MyOptions = nil) or (MyLog = nil) then
    exit;

  // Construct a string stream for options
  OptionsStream := ATextOutputStream.around(AStringStream.new);

  // Loop and add each option
  for i := 1 to System.paramCount do
  begin
    if i > 1 then
      OptionsStream.printEach([' ', System.paramStr(i)])

    else
      OptionsStream.print(System.paramStr(i));
  end;

  //OptionsStream.print(#26);
  OptionsStream.setPosition(0);

  // Parse the options
  CommandLine := ALinearCParser.forSource(OptionsStream.Target, MyOptions,
    MyLog);
  result := CommandLine.parse;

  CommandLine.free;
  OptionsStream.free;
end;

// Run the program
function ALinearCConsoleApplication.run: integer;

begin
  result := inherited run;

  if result <> 0 then
    exit;

  result := -1;

  // Construct a dictionary of command-line options, if necessary
  if MyOptions = nil then
    MyOptions := ACommandLineOptionDictionary.new;

  // Construct a log, if necessary
  if MyLog = nil then
    MyLog := ALog.toStream(MyErrorOut);

  if (MyOptions = nil) or (MyLog = nil) then
    exit;

  // Build the dictionary of command-line options accepted by the program
  Self.defineOptions;
  // Parse the command line
  if System.paramCount > 0 then
    result := integer(Self.parseCommandLine);

  if result <> 0 then
    exit;

  // Check to see if '--version' or '--help' were specified
  if MyOptions.valueOf(lckwHelpLongString, false) then
    Self.printHelpTo(MyConsoleOut)

  else if MyOptions.valueOf(lckwVersionLongString, false) then
    Self.printVersionTo(MyConsoleOut, lcappVersionDefaultAdditionalItems);

  result := 0;
end;

// Print usage information to the specified stream
procedure ALinearCConsoleApplication.printHelpTo(const ThisStream: ATextOutputStream);

var
  // Refers to the list of rest argument names
  RestArguments: AStringList;

begin
  if (ThisStream = nil) or (MyOptions = nil) then
    exit;

  RestArguments := MyOptions.RestArgumentNames;
  RestArguments.sort;

  ThisStream.print(SysUtils.format(lcappUsageStringFormat, [
    Self.executableName(true),
    RestArguments.implode(' ')
  ]));

  RestArguments.free;

  MyOptions.printUsageTo(ThisStream);
  // Add an extra line before we return to the command prompt
  ThisStream.print(System.lineEnding);
end;

// Print version information to the specified stream
procedure ALinearCConsoleApplication.printVersionTo(const ThisStream: ATextOutputStream;
  const additionalItems: array of string);

var
  // Loop counters
  i, j: longword;
  // Stores the name of a key
  key: string;
  // Stores the value of a key
  value: Variant;

begin
  if (ThisStream = nil) or (MyOptions = nil) then
    exit;

  // Print the name of the program and its version
  ThisStream.print(SysUtils.format(lcappVersionStringFormat, [
    Self.VersionInfo.valueOf(vinfProductName, ''),
    Self.VersionInfo.valueOf(vinfFileVersion, '')
  ]));

  // Print additional items, if specified
  for i := 0 to System.high(additionalItems) do
  begin
    // Check to see whether the string represents a version info key
    key := additionalItems[i];
    value := Self.VersionInfo.valueOf(key);

    if value <> Variants.null then
      ThisStream.printWrapped(Variants.varToStr(value),
        tosWrapDefaultBreakCharacters)

    else begin
      // This will flag whether the value was whitespace and was printed
      value := false;

      // Check to see whether the string represents whitespace or a line break
      for j := 0 to System.high(tosWrapDefaultBreakCharacters) do
        if key = tosWrapDefaultBreakCharacters[j] then
        begin
          ThisStream.print(key);
          value := true;
          break;
        end;

      if value = false then
        ThisStream.printWrapped(key, tosWrapDefaultBreakCharacters);
    end;
  end;
end;

// Properties ------------------------------------------------------------------
// Retrieve a reference to the list of options
function ALinearCConsoleApplication.Options: ACommandLineOptionDictionary;

begin
  Result := MyOptions;
end;

// Retrieve a reference to the log used for error and diagonstic output
function ALinearCConsoleApplication.Log: ALog;

begin
  Result := MyLog;
end;

{$ENDIF}

