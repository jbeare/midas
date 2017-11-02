use strict;
use warnings;

use Data::Dumper qw(Dumper);
use File::Basename;
use Getopt::Long;
use Time::Piece;

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
    # This does not take into account timezones.
    my $timestamp = shift;
    my ($sec, $min, $hour, $mday, $mon, $year, $wday, $yday, $isdst) = localtime($timestamp);
    return ($sec + ($min * 60) + ($hour * 3600)) / 86400;
}

sub GetTimeOfWeek
{
    # This does not take into account timezones.
    my $timestamp = shift;
    my ($sec, $min, $hour, $mday, $mon, $year, $wday, $yday, $isdst) = localtime($timestamp);
    return ($min + ($hour * 60) + ($wday * 1440)) / 10080;
}

sub GetTimeOfYear
{
    # This does not take into account timezones.
    my $timestamp = shift;
    my ($sec, $min, $hour, $mday, $mon, $year, $wday, $yday, $isdst) = localtime($timestamp);
    return $yday / 365;
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
        
        #push @observation, GetDelta($raw[$i - 1][4], $raw[$i - 1][1]);
        #push @observation, GetDelta($raw[$i - 2][4], $raw[$i - 2][1]);
        #push @observation, GetDelta($raw[$i - 3][4], $raw[$i - 3][1]);
        #push @observation, GetDelta($raw[$i - 4][4], $raw[$i - 4][1]);
        #push @observation, GetDelta($raw[$i - 5][4], $raw[$i - 5][1]);
        
        #push @observation, GetDelta($raw[$i][4], $raw[$i - 1][1]);  
        #push @observation, GetDelta($raw[$i][4], $raw[$i - 2][1]);  
        #push @observation, GetDelta($raw[$i][4], $raw[$i - 3][1]);  
        #push @observation, GetDelta($raw[$i][4], $raw[$i - 4][1]);  
        #push @observation, GetDelta($raw[$i][4], $raw[$i - 5][1]);  
        
        # top 5 winners
        # d=4 1 2 7 9
        # d=4 1 2 7 8
        # d=5 0 1 2 7 8
        # d=5 0 1 2 7 8 9
        push @observation, GetTimeOfDay($raw[$i][0]); # 0
        push @observation, GetDelta($raw[$i][4], $raw[$i][1]); # 1
        push @observation, GetDelta($raw[$i][2], $raw[$i][3]); # 2
        push @observation, GetDelta($raw[$i - 5][2], $raw[$i - 5][3]); # 7
        push @observation, GetDelta($raw[$i][1], $raw[$g_observationDepth][1]); # 8
        push @observation, GetDelta($raw[$i][4], $raw[$g_observationDepth][4]); # 9
        # d=5 0 1 2 7 9
        
        #XX 0 : 1659
        # 1 : 1321
        # 2 : 1342
        #push @observation, GetTimeOfDay($raw[$i][0]); # 0
        #push @observation, GetTimeOfWeek($raw[$i][0]);
        #push @observation, GetTimeOfYear($raw[$i][0]);
        
        #XX 3 : 19313
        #XX 4 : 2850
        # 5 : 712
        #XX 6 : 1624
        # 7 : 1087
        #XX 8 : 1804
        # 9 : 572
        #XX 10 : 1631
        # 11 : 645
        #XX 12 : 1770
        # 13 : 563
        #XX 14 : 1828
        #push @observation, GetDelta($raw[$i][4], $raw[$i][1]); # 1
        #push @observation, GetDelta($raw[$i][2], $raw[$i][3]); # 2
        #push @observation, GetDelta($raw[$i - 1][4], $raw[$i - 1][1]);
        #push @observation, GetDelta($raw[$i - 1][2], $raw[$i - 1][3]); # 3
        #push @observation, GetDelta($raw[$i - 2][4], $raw[$i - 2][1]);
        #push @observation, GetDelta($raw[$i - 2][2], $raw[$i - 2][3]); # 4
        #push @observation, GetDelta($raw[$i - 3][4], $raw[$i - 3][1]);
        #push @observation, GetDelta($raw[$i - 3][2], $raw[$i - 3][3]); # 5
        #push @observation, GetDelta($raw[$i - 4][4], $raw[$i - 4][1]);
        #push @observation, GetDelta($raw[$i - 4][2], $raw[$i - 4][3]); # 6
        #push @observation, GetDelta($raw[$i - 5][4], $raw[$i - 5][1]);
        #push @observation, GetDelta($raw[$i - 5][2], $raw[$i - 5][3]); # 7
        
        #XX 15 : 1974
        # 16 : 1363
        # 17 : 1366
        #XX 18 : 1968
        # 19 : 1303
        #push @observation, GetDelta($raw[$i][1], $raw[$g_observationDepth][1]); # 8
        #push @observation, GetDelta($raw[$i][2], $raw[$g_observationDepth][2]);
        #push @observation, GetDelta($raw[$i][3], $raw[$g_observationDepth][3]);
        #push @observation, GetDelta($raw[$i][4], $raw[$g_observationDepth][4]); # 9
        #push @observation, GetDelta($raw[$i][5], $raw[$g_observationDepth][5]);
        
        for(my $j = 1; $j <= $g_observationDepth; $j++)
        {
            # 20 : 73
            # 21 : 8
            # 22 : 13
            # 23 : 1240
            # 24 : 146
            # 25 : 19
            # 26 : 46
            # 27 : 86
            # 28 : 1023
            # 29 : 672
            # 30 : 308
            # 31 : 366
            # 32 : 214
            # 33 : 1707
            # 34 : 574
            # 35 : 572
            # 36 : 853
            # 37 : 742
            # 38 : 2044
            # 39 : 434
            # 40 : 550
            # 41 : 728
            # 42 : 591
            # 43 : 1891
            # 44 : 641
            #push @observation, GetDelta($raw[$i][1], $raw[$i - $j][1]);
            #push @observation, GetDelta($raw[$i][2], $raw[$i - $j][2]);
            #push @observation, GetDelta($raw[$i][3], $raw[$i - $j][3]);
            #push @observation, GetDelta($raw[$i][4], $raw[$i - $j][1]);
            #push @observation, GetDelta($raw[$i][5], $raw[$i - $j][5]);
        }
        
        for(my $j = 0; $j < $g_observationDepth; $j++)
        {
            # 45 : 73
            # 46 : 8
            # 47 : 13
            # 48 : 867
            # 49 : 146
            # 50 : 1292
            # 51 : 1111
            # 52 : 1303
            # 53 : 1220
            # 54 : 1139
            # 55 : 1097
            # 56 : 838
            # 57 : 698
            # 58 : 659
            # 59 : 908
            # 60 : 678
            # 61 : 502
            # 62 : 368
            # 63 : 1224
            # 64 : 931
            # 65 : 1045
            # 66 : 564
            # 67 : 979
            # 68 : 1341
            # 69 : 980
            #push @observation, GetDelta($raw[$i - $j][1], $raw[$i - $j - 1][1]);
            #push @observation, GetDelta($raw[$i - $j][2], $raw[$i - $j - 1][2]);
            #push @observation, GetDelta($raw[$i - $j][3], $raw[$i - $j - 1][3]);
            #push @observation, GetDelta($raw[$i - $j][4], $raw[$i - $j - 1][4]);
            #push @observation, GetDelta($raw[$i - $j][5], $raw[$i - $j - 1][5]);
        }
        
        push @observations, [@observation];
        
        #push(@labels, ($raw[$i + 1][4] > $raw[$i][4]) + 0); # result
        
        #push(@labels, [
        #    ($raw[$i + 1][2] > ($raw[$i][4] + 0.005)) + 0,
        #    ($raw[$i + 1][2] > ($raw[$i][4] + 0.01)) + 0,
        #    ($raw[$i + 1][2] > ($raw[$i][4] + 0.05)) + 0,
        #    ($raw[$i + 1][2] > ($raw[$i][4] + 0.10)) + 0,
        #    ($raw[$i + 1][2] > ($raw[$i][4] + 0.15)) + 0,
        #    ($raw[$i + 1][2] > ($raw[$i][4] + 0.20)) + 0,
        #    ($raw[$i + 1][2] > ($raw[$i][4] + 0.50)) + 0,
        #    ]);
        
        my $delta = $raw[$i + 1][2] - $raw[$i][4];
        
        if($delta > 0.25)
        {
            push(@labels, 7);
        }
        elsif($delta > 0.20)
        {
            push(@labels, 6);
        }
        elsif($delta > 0.15)
        {
            push(@labels, 5);
        }
        elsif($delta > 0.10)
        {
            push(@labels, 4);
        }
        elsif($delta > 0.05)
        {
            push(@labels, 3);
        }
        elsif($delta > 0.01)
        {
            push(@labels, 2);
        }
        elsif($delta > 0.005)
        {
            push(@labels, 1);
        }
        else
        {
            push(@labels, 0);
        }
        
        print $rawFile join(',', @{$raw[$i]}) . "\n";
    }

    #print Dumper \@observations;

    for my $observation (@observations)
    {
        print $writeFile join(',', @$observation) . "\n";
    }
    
    #for my $label (@labels)
    #{
    #    print $labelFile join(',', @$label) . "\n";
    #}

    print $labelFile join(',', @labels);
        
    close($readFile);
    close($writeFile);
    close($labelFile);
}
    
GetObservations($g_inputPath);
