var ViewMainPage = /** @class */ (function () {
    function ViewMainPage(myf) {
        this.myf = myf;
    }
    ViewMainPage.prototype.showDevices = function (list) {
        // cargo la lista de objetos en el DOM
        var devicesUl = this.myf.getElementById("devicesList");
        var items = "";
        for (var i in list) {
            var checkedStr = "";
            if (list[i].state == "1")
                checkedStr = "checked";
            switch (list[i].type) {
                case "0": // Lampara                     
                    items += "<li class='collection-item avatar'> \
                                <img src='images/lightbulb.png' alt='' class='circle'> \
                                <span class='title'>" + list[i].name + "</span> \
                                <p>" + list[i].description + "<br> \
                                </p> \
                                <a href='#!' class='secondary-content'> <div class='switch'> \
                                                                            <label> \
                                                                            Off \
                                                                            <input type='checkbox' id='dev_" + list[i].id + "' " + checkedStr + "> \
                                                                            <span class='lever'></span> \
                                                                            On \
                                                                            </label> \
                                                                        </div></a> \
                            </li>";
                    break;
                case "1": // Persiana                    
                    items += "<li class='collection-item avatar'> \
                                <img src='images/window.png' alt='' class='circle'> \
                                <span class='title'>" + list[i].name + "</span> \
                                <p>" + list[i].description + "<br> \
                                </p> \
                                <a href='#!' class='secondary-content'> <div class='switch'> \
                                                                            <label> \
                                                                            Off \
                                                                            <input type='checkbox' id='dev_" + list[i].id + "' " + checkedStr + "> \
                                                                            <span class='lever'></span> \
                                                                            On \
                                                                            </label> \
                                                                        </div></a> \
                            </li>";
                    break;
            }
        }
        devicesUl.innerHTML = items;
    };
    ViewMainPage.prototype.getSwitchStateById = function (id) {
        var el = this.myf.getElementById(id);
        return el.checked;
    };
    return ViewMainPage;
}());
var Main = /** @class */ (function () {
    function Main() {
    }
    Main.prototype.handleEvent = function (evt) {
        var sw = this.myf.getElementByEvent(evt);
        console.log("click en device:" + sw.id);
        var data = { "id": sw.id, "state": this.view.getSwitchStateById(sw.id) };
        this.myf.requestPOST("cgi-bin/setLamp.py", data, this);
    };
    Main.prototype.handleGETResponse = function (status, response) {
        if (status == 200) {
            var data = JSON.parse(response);
            this.view.showDevices(data);
            for (var i in data) {
                var sw = this.myf.getElementById("dev_" + data[i].id);
                sw.addEventListener("click", this);
            }
        }
    };
    Main.prototype.handlePOSTResponse = function (status, response) {
        if (status == 200) {
            console.log(response);
        }
    };
    Main.prototype.main = function () {
        var _this = this;
        this.myf = new MyFramework();
        this.view = new ViewMainPage(this.myf);
        setInterval(function () { _this.myf.requestGET("cgi-bin/getDevices.py", _this); }, 1000);
    };
    return Main;
}());
window.onload = function () {
    var obj = new Main();
    obj.main();
};
