<?php
function show($file_path) {
    $command = sprintf ( "tree %s", filesize ( $file_path ) );
    // 执行命令并打开命令的输出指针
    $handle = popen($command, 'r');
    $cmd_content = ' ';
    while (!feof($handle)) {
        // 循环获取命令输出
        $data = fgets($handle);
        // 刷新输出缓冲区
        ob_flush();
        flush();
        $cmd_content.$data;
    }
    echo $cmd_content;
    // 命令执行结束, 关闭指针
    pclose($handle);
}
?>