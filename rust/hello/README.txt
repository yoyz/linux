https://www.server-world.info/en/note?os=CentOS_8&p=rust&f=1

[root@dlp ~]# mkdir rust_test
[root@dlp ~]# cd rust_test
[root@dlp rust_test]# cat > hello_rust.rs << EOF
fn main() {
    println!("Hello Rust world !");
}
EOF

[root@dlp rust_test]# rustc hello_rust.rs
[root@dlp rust_test]# ./hello_rust
Hello Rust world !
# verify to create Hello World !
