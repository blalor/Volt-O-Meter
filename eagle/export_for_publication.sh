#!/bin/bash -e

_EAGLE="$HOME/Applications/EAGLE/EAGLE.app/Contents/MacOS/EAGLE"
board_dir="$( cd $(dirname $0) && /bin/pwd )"

cd "${board_dir}"

tmpf=$( mktemp -t exportXXXX )
mv ${tmpf} ${tmpf}.scr
tmpf="${tmpf}.scr"

for x in *.sch; do
    rm -f ${x}.png ${x%.sch}.parts.txt
    
    echo "edit ${x};" >> ${tmpf}
    echo "export partlist '${x%.sch}.parts.txt';" >> ${tmpf}
    echo "EXPORT IMAGE '${x}.png' 300;" >> ${tmpf}
done

_first_board=""

for x in *.brd; do
    if [ -z "${_first_board}" ]; then
        _first_board="${x}"
    fi
    
    rm -f ${x}.png
    
    echo "edit ${x};" >> ${tmpf}
    echo "ratsnest;" >> ${tmpf}
    echo "EXPORT IMAGE '${x}.png' 300;" >> ${tmpf}
done

echo "quit;" >> ${tmpf}

"${_EAGLE}" -C "script ${tmpf}" ${_first_board}

## parts list gets exported as iso-8859-1
for x in *.parts.txt; do
    iconv -f iso-8859-1 -t utf-8 ${x} | expand | egrep -v '^Exported from ' > ${x}.converted
    mv ${x}.converted ${x}
done

rm -f ${tmpf}
