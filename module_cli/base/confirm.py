import sys
def confirm_or_exit(confirm_text : str = "Confirm"):
    confirm = input(f'{confirm_text} [Y/N] : ')
    if confirm.strip().lower() != 'y':
        sys.exit(1)
