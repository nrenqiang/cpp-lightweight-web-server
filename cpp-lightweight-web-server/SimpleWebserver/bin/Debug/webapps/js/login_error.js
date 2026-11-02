const params = new URLSearchParams(window.location.search);
if (params.get("error") === "1") {
    alert("账号或密码错误，请重新输入！");
}