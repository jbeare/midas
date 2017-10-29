use strict;
use warnings;

use Data::Dumper qw(Dumper);
use File::Basename;
use Getopt::Long;

my $g_inputPath = "";
my $g_observationDepth = 5;

GetOptions(
    "path=s"   => \$g_inputPath,
    "depth=i" => \$g_observationDepth)
    or die("Failed to parse command line arguments.\n");

sub ParseInputFile
{
    my $file = shift;
    my @data;
    while (my $line = readline($file))
    {
        $line =~ s/\s+$//;
        push(@data, [split(/,/, $line)]);
    }
    return @data;
}

sub GetTimeOfDay
{

}

sub GetTimeOfWeek
{

}

sub GetTimeOfYear
{

}

sub GetDelta
{
    my $a = shift;
    my $b = shift;
    my $c = ((($a - $b) / $b) / 2) + .5;
    return $c;
}

# Observations:
#    - Time of Day
#    - Time of Week
#    - Time of Year
#    - OBSERVATION of MEASUREMENT X and MEASUREMENT X-Y
#        ○ X = now; Y = [1..Z]
#        ○ MEASUREMENT = open, high, low, close, volume
#        ○ OBSERVATION = delta, volatility
#    - OBSERVATION of MEASUREMENT X and MEASUREMENT X-1
#        ○ X = [now..Z]
#        ○ MEASUREMENT = open, high, low, close, volume
#        ○ OBSERVATION = delta, volatility
sub GetObservations
{
    my $fileName = shift;
    my ($name, $path, $suffix) = fileparse($fileName, '\.[^\.]*');
    my $readFile;
    my $writeFile;
    my $labelFile;
    my $rawFile;

    open($readFile, "<", $fileName) or die "Couldn't open $fileName";
    open($writeFile, ">", "${name}_observations.csv") or die "Couldn't open ${name}_observations.csv";
    open($labelFile, ">", "${name}_labels.csv") or die "Couldn't open ${name}_labels.csv";
    open($rawFile, ">", "${name}_raw.csv") or die "Couldn't open ${name}_raw.csv";

    # 0:timestamp, 1:open, 2:high, 3:low, 4:close, 5:volume
    my @raw = ParseInputFile($readFile); 
    my @observations;
    my @labels;

    # print Dumper \@raw;
    
    for(my $i = $g_observationDepth; $i < (scalar @raw) - $g_observationDepth; $i++)
    {
        my @observation;
        
        #push @observation, GetDelta($raw[$i][4], $raw[$i - 3][1]);
        
        #good 94
        #push @observation, GetDelta($raw[$i][4], $raw[$i][1]);
        
        #good 90
        #push @observation, GetDelta($raw[$i][1], $raw[$i - 1][1]);
        
        #bad 50
        #push @observation, GetDelta($raw[$i][4], $raw[$i - 1][4]);
        
        #bad 71
        #push @observation, GetDelta(($raw[$i][4] + $raw[$i][1]) / 2, ($raw[$i - 1][4] + $raw[$i - 1][1]) / 2);
        
        #push(@observation, $raw[$i][0]/1700000000); # relative recentness of event; normalized unix timestamp between 0 and 1700000000 (Jan 01 1970 and 11/14/2023)
        #push(@observation, ((($raw[$i][4] - $raw[$i][1]) / $raw[$i][1]) / 2) + .5); # Relative delta in open/close for current event n-x ((c - o)/o)/2+.5
        #push(@observation, ((($raw[$i - 1][4] - $raw[$i - 1][1]) / $raw[$i - 1][1]) / 2) + .5); # Relative delta in open/close for current event n-x ((c - o)/o)/2+.5
        #push(@observation, ((($raw[$i - 2][4] - $raw[$i - 2][1]) / $raw[$i - 2][1]) / 2) + .5); # Relative delta in open/close for current event n-x ((c - o)/o)/2+.5
        #push(@observation, ((($raw[$i - 3][4] - $raw[$i - 3][1]) / $raw[$i - 3][1]) / 2) + .5); # Relative delta in open/close for current event n-x ((c - o)/o)/2+.5
        #push(@observation, ((($raw[$i - 4][4] - $raw[$i - 4][1]) / $raw[$i - 4][1]) / 2) + .5); # Relative delta in open/close for current event n-x ((c - o)/o)/2+.5
        #push(@observation, ((($raw[$i - 5][4] - $raw[$i - 5][1]) / $raw[$i - 5][1]) / 2) + .5); # Relative delta in open/close for current event n-x ((c - o)/o)/2+.5
        #push(@observation, ((($raw[$i][4] - $raw[$i - 1][1]) / $raw[$i - 1][1]) / 2) + .5); # Relative delta in open/close for current event n-1 ((c - o)/o)/2+.5
        #push(@observation, ((($raw[$i][4] - $raw[$i - 2][1]) / $raw[$i - 2][1]) / 2) + .5); # Relative delta in open/close for current event n-1 ((c - o)/o)/2+.5
        #push(@observation, ((($raw[$i][4] - $raw[$i - 3][1]) / $raw[$i - 3][1]) / 2) + .5); # Relative delta in open/close for current event n-1 ((c - o)/o)/2+.5
        #push(@observation, ((($raw[$i][4] - $raw[$i - 4][1]) / $raw[$i - 4][1]) / 2) + .5); # Relative delta in open/close for current event n-1 ((c - o)/o)/2+.5
        #push(@observation, ((($raw[$i][4] - $raw[$i - 5][1]) / $raw[$i - 5][1]) / 2) + .5); # Relative delta in open/close for current event n-1 ((c - o)/o)/2+.5
        
        push @observation, GetDelta($raw[$i][4], $raw[$i][1]);
        
        push @observation, GetDelta($raw[$i - 1][4], $raw[$i - 1][1]);
        push @observation, GetDelta($raw[$i - 2][4], $raw[$i - 2][1]);
        push @observation, GetDelta($raw[$i - 3][4], $raw[$i - 3][1]);
        push @observation, GetDelta($raw[$i - 4][4], $raw[$i - 4][1]);
        push @observation, GetDelta($raw[$i - 5][4], $raw[$i - 5][1]);
        
        push @observation, GetDelta($raw[$i][4], $raw[$i - 1][1]);  
        push @observation, GetDelta($raw[$i][4], $raw[$i - 2][1]);  
        push @observation, GetDelta($raw[$i][4], $raw[$i - 3][1]);  
        push @observation, GetDelta($raw[$i][4], $raw[$i - 4][1]);  
        push @observation, GetDelta($raw[$i][4], $raw[$i - 5][1]);  

        #push @observation, GetTimeOfDay($raw[$i][0]);
        #push @observation, GetTimeOfWeek($raw[$i][0]);
        #push @observation, GetTimeOfYear($raw[$i][0]);
        
        #push @observation, GetDelta($raw[$i][1], $raw[$g_observationDepth][1]);
        #push @observation, GetDelta($raw[$i][2], $raw[$g_observationDepth][2]);
        #push @observation, GetDelta($raw[$i][3], $raw[$g_observationDepth][3]);
        #push @observation, GetDelta($raw[$i][4], $raw[$g_observationDepth][4]);
        #push @observation, GetDelta($raw[$i][5], $raw[$g_observationDepth][5]);
        
        for(my $j = 1; $j <= $g_observationDepth; $j++)
        {
            #push @observation, GetDelta($raw[$i][1], $raw[$i - $j][1]);
            #push @observation, GetDelta($raw[$i][2], $raw[$i - $j][2]);
            #push @observation, GetDelta($raw[$i][3], $raw[$i - $j][3]);
            #push @observation, GetDelta($raw[$i][4], $raw[$i - $j][1]);
            #push @observation, GetDelta($raw[$i][5], $raw[$i - $j][5]);
        }
        
        for(my $j = 0; $j < $g_observationDepth; $j++)
        {
            #push @observation, GetDelta($raw[$i - $j][1], $raw[$i - $j - 1][1]);
            #push @observation, GetDelta($raw[$i - $j][2], $raw[$i - $j - 1][2]);
            #push @observation, GetDelta($raw[$i - $j][3], $raw[$i - $j - 1][3]);
            #push @observation, GetDelta($raw[$i - $j][4], $raw[$i - $j - 1][4]);
            #push @observation, GetDelta($raw[$i - $j][5], $raw[$i - $j - 1][5]);
        }
        
        push @observations, [@observation];
        
        #push(@labels, ($raw[$i + 1][4] > $raw[$i][4]) + 0); # result
        
        push(@labels, [
            ($raw[$i + 1][2] > ($raw[$i][4] + 0.005)) + 0,
            ($raw[$i + 1][2] > ($raw[$i][4] + 0.01)) + 0,
            ($raw[$i + 1][2] > ($raw[$i][4] + 0.05)) + 0,
            ($raw[$i + 1][2] > ($raw[$i][4] + 0.10)) + 0,
            ($raw[$i + 1][2] > ($raw[$i][4] + 0.15)) + 0,
            ($raw[$i + 1][2] > ($raw[$i][4] + 0.20)) + 0,
            ($raw[$i + 1][2] > ($raw[$i][4] + 0.50)) + 0,
            ]);
        
        print $rawFile join(',', @{$raw[$i]}) . "\n";
    }

    #print Dumper \@observations;

    for my $observation (@observations)
    {
        print $writeFile join(',', @$observation) . "\n";
    }
    
    for my $label (@labels)
    {
        print $labelFile join(',', @$label) . "\n";
    }

    #print $labelFile join(',', @labels);
        
    close($readFile);
    close($writeFile);
    close($labelFile);
}
    
GetObservations($g_inputPath);
