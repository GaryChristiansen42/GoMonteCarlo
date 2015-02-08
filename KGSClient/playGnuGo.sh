#python ../twogtp.py --black './GTPClient | tee testOutput' --white 'gnugo --mode gtp --boardsize 9 --chinese-rules --level 1' --size 9 --games 10
#python ../twogtp.py --black 'gnugo --mode gtp --boardsize 9 --chinese-rules --level 1' --white './GTPClient | tee testOutput' --size 9 --games 10

#echo "GnuGoVsGnuGoLevel10-9x9"
#python ../twogtp.py --black 'gnugo --mode gtp --boardsize 9 --chinese-rules --level 1' --white 'gnugo --mode gtp --boardsize 9 --chinese-rules --level 10' --size 9 --games 100 
#echo "GnuGoVsGnuGoLevel10-19x19"
#python ../twogtp.py --black 'gnugo --mode gtp --boardsize 19 --chinese-rules --level 1' --white 'gnugo --mode gtp --boardsize 19 --chinese-rules --level 10' --size 19 --games 100 
echo "GnuGoVsGnuGoLevel10-9x9"
python ../twogtp.py --black './GTPClient | tee testOutput' --white 'gnugo --mode gtp --boardsize 9 --chinese-rules --level 10' --size 9 --games 100 
echo "End"
