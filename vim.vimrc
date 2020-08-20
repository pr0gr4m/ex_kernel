set hlsearch
set nu
set autoindent
set scrolloff=2
set wildmode=longest,list
set ts=8
set sts=8
set sw=1
set cindent
set autowrite
set autoread
set bs=eol,start,indent
set history=256
set laststatus=2
set shiftwidth=8
set smartcase
set smarttab
set smartindent
set softtabstop=8
set tabstop=8
set ruler
set incsearch
set fileencodings=utf-8,euc-kr
set nobackup
set title
set nowrap
set wmnu
set nocompatible
set background=dark

set tags+=/usr/src/linux-5.7.2/tags
set tags+=~/stdlib.tag
set tagbsearch

let Tlist_Display_Tag_Scope=1
let Tlist_Sort_Type="name"

map <F2> :x
map <F3> <c-w><c-w>
map <F4> :Tlist<cr>
map <F5> :w<cr> :make<cr> :ccl<cr> :cw<cr>

au BufReadPost *
\ if line("'\"") > 0 && line("'\"") <= line("$") |
\ exe "norm g`\"" |
\ endif

syntax on
colorscheme gruvbox

autocmd FileType * setlocal comments-=://
