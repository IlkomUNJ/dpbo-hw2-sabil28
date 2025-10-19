# File: meson.build
project('ECommerceSimulator', 'cpp',
    version : '1.0',
    default_options : [
        'cpp_std=c++17',     
        'warning_level=3'     
    ]
)

app_sources = [
    'main.cpp',
    'DataTypes.cpp',
    'BankAccount.cpp',
    'Item.cpp',
    'Transaction.cpp',
    'User.cpp',
    'Buyer.cpp',
    'Seller.cpp',
    'Bank.cpp',
    'Store.cpp'
]

project_includes = include_directories('.')

executable('simulator',
    app_sources,
    include_directories: project_includes,
    install : true 
)