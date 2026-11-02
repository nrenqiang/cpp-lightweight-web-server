/**
 * 注册表单客户端验证
 * 返回 false 阻止提交，返回 true 允许提交到 DLL
 */
function validateRegister() {
    var username = document.getElementById("username").value.trim();
    var phone = document.getElementById("phone").value.trim();
    var password = document.getElementById("password").value.trim();
    var confirmPwd = document.getElementById("confirmPwd").value.trim();

    if (!username) {
        alert("请输入用户名！");
        return false;
    }
    if (!phone) {
        alert("请输入手机号！");
        return false;
    }
    if (!password) {
        alert("请输入密码！");
        return false;
    }
    if (password.length < 6 || password.length > 16) {
        alert("密码长度需要 6-16 位！");
        return false;
    }
    if (password !== confirmPwd) {
        alert("两次输入的密码不一致！");
        return false;
    }
    return true;
}

// 保留旧 register() 函数以兼容，内部调用验证
function register() {
    if (validateRegister()) {
        document.forms[0].submit();
    }
}
