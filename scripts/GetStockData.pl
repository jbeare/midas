use strict;
use warnings;

use LWP::Simple;

my $periodInSeconds = 60;
my $intervalInDays = 10;
	
sub QueryGoogleForData
{
	my $tickerSymbol = shift;
	my $url = "http://www.google.com/finance/getprices?i=$periodInSeconds&p=${intervalInDays}d&f=d,o,h,l,c,v&df=cpct&q=$tickerSymbol";
	print $url;
	my $content = get $url;
	die "Couldn't get $url" unless defined $content;
	return $content;
}

sub RemoveHeader
{
	my $content = shift;
	$content =~ s/.*TIMEZONE_OFFSET[^\n]*\n//smg;
	return $content;
}

sub FixTimestamps
{
	my $content = shift;
	my @contentChunks = ($content =~ m/^(a[^a]*)/smg);
	for my $contentChunk (@contentChunks)
	{
		$contentChunk =~ m/^a([^,]*)/;
		my $timestamp = $1 - $periodInSeconds;
		$contentChunk =~ s/^([^,]*)/$timestamp+=$periodInSeconds/mge;
	}
	return join("", @contentChunks);
}

sub UpdateStockDataFile
{
	my $tickerSymbol = shift;
	my $lastLine = "";
	if(not -d "Stocks")
	{
		mkdir "Stocks";
	}
	if (open(my $readFile, "<", "Stocks\\$tickerSymbol.txt"))
	{
		while (my $line = readline($readFile))
		{
			if ($line =~ /\S/)
			{
				$lastLine = $line;
			}
		}
		close($readFile);
	}
	$lastLine =~ m/^([^,]*)/;
	my $lastTimestamp = $1;
	my $content = FixTimestamps(RemoveHeader(QueryGoogleForData($tickerSymbol)));
	if ((defined $lastTimestamp) and !($lastTimestamp eq ""))
	{
		$content =~ s/.*$lastTimestamp[^\n]*\n//smg;
	}
	if (open(my $writeFile, ">>", "Stocks\\$tickerSymbol.txt"))
	{
		print $writeFile $content;
		close($writeFile);
	}
}

sub UpdateAllStockDataFiles
{
	my $content = "";
	if (open(my $readFile, "<", "Stocks.txt"))
	{
		local $/;
		$content = <$readFile>;
		close($readFile);
	}
	my @symbols = ($content =~ m/[A-Z]*/smg);
	for my $symbol (@symbols)
	{
		if(!($symbol eq ""))
		{
			UpdateStockDataFile($symbol);
		}
	}
}

#print QueryGoogleForData("AAPL");
#UpdateStockDataFile(60, 1, "IBM");
UpdateAllStockDataFiles;