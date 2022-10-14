ps -o comm= -p $$

VF_RW_WD=`pwd`
SOURCE_FILE="$1"
BACKUP_IDX=0


# IMPORTANT:
# None of the provided regexes must contain the unescaped character '|'
#
# $1 : sed 'find' regex
# $2 : sed 'replace' regex
rewrite()
{
  FIND_REGEX=$1
  REPLACE_REGEX=$2
  echo "VF RW: \"$FIND_REGEX\" -> \"$REPLACE_REGEX\""
  echo "backup : $VF_RW_WD/$SOURCE_FILE.backup-$BACKUP_IDX"
  sed -i."backup-$BACKUP_IDX" "s|$FIND_REGEX|$REPLACE_REGEX|g" $SOURCE_FILE
  ((BACKUP_IDX=BACKUP_IDX+1))
  echo "\n"
}

echo "Commenting out line/file pragmas"
rewrite "^#" "// &"

rewrite "long unsigned int" "unsigned long int"

#echo "VF RW: 'long unsigned int' -> 'unsinged long int'"
#echo "backup : $VF_RW_WD/$SOURCE_FILE.backup-$BACKUP_IDX"
#echo backup index $BACKUP_IDX
#sed -i."backup-$BACKUP_IDX" 's|long unsigned int|unsigned long int|g' $SOURCE_FILE
#((BACKUP_IDX=BACKUP_IDX+1))
#echo backup index $BACKUP_IDX
