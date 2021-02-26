#  By convention, the function name
#+ starts with an underscore.
_asm-comp() {

    # Pointer to current completion word.
    # By convention, it's named "cur" but this isn't strictly necessary.
    local cur

    # Array variable storing the possible completions.
    COMPREPLY=()
    
    cur=${COMP_WORDS[COMP_CWORD]}

    # Generate the completion matches and load them into $COMPREPLY array.
    if [[ "$cur" == "--"* ]]
    then
        COMPREPLY=( $( compgen -W "--dev --help --labels --version" -- "$cur" ) )
    elif [[ "$cur" == "-"* ]]
    then
        COMPREPLY=( $( compgen -W "-d -h -l -v" -- "$cur" ) )  
    fi
    return 0
}

complete -F _asm-comp -o default asm
