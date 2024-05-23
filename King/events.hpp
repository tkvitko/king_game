//
//  events.hpp
//  King
//
//  Created by tkvitko on 10.05.2024.
//

#include <string>
#include <vector>
#include <algorithm>
#include <random>

enum class EventType {
    good,
    bad
};

enum class ChangingField {
    balance,
    balance_by_koef_per_countryman,
    countryman,
    countryman_next_year,
    countryman_by_koef_per_countrymen,
    
    tourism_percentage,
    harvest_percentage,
    cost_of_life_percentage
};

class Event {
public:
    EventType type;
    std::string text;
    int change_balance = 0;
    int change_countryman = 0;
    int change_countryman_next_year = 0;
    int change_balance_by_koef_per_countryman = 0;
    int change_countryman_by_koef_per_countrymen = 0;
    
    int change_tourism_percentage = 0;
    int change_harvest_percentage = 0;
    int change_cost_of_life_percentage = 0;
    
    Event() {};
    
    Event(std::string text_, std::vector<std::pair<ChangingField, int>> params) {
        text = text_;
        for (auto& param : params) {
            ChangingField field = param.first;
            int value = param.second;
            
            switch (field) {
                case ChangingField::balance:
                    change_balance = value;
                    break;
                case ChangingField::countryman:
                    change_countryman = value;
                    break;
                case ChangingField::tourism_percentage:
                    change_tourism_percentage = value;
                    break;
                case ChangingField::balance_by_koef_per_countryman:
                    change_balance_by_koef_per_countryman = value;
                    break;
                case ChangingField::countryman_by_koef_per_countrymen:
                    change_countryman_by_koef_per_countrymen = value;
                    break;
                case ChangingField::countryman_next_year:
                    change_countryman_next_year = value;
                    break;
                case ChangingField::cost_of_life_percentage:
                    change_cost_of_life_percentage = value;
                    break;
                case ChangingField::harvest_percentage:
                    change_harvest_percentage = value;
                    break;
            }
        }
    }
};

class Quiz {
public:
    std::string question;
    std::string neutral_answer;
    Event good_event;
    Event bad_event;
    
    Quiz(std::string question_, std::string neutral_answer_,
         Event good_event_, Event bad_event_) {
        question = question_;
        neutral_answer = neutral_answer_;
        good_event = good_event_;
        bad_event = bad_event_;
    }
};

class Quizes {
private:
    std::vector<Quiz> quizes;
    
public:
    Quizes() {
        std::vector<std::pair<ChangingField, int>> good_params_1 = {std::pair<ChangingField, int>(ChangingField::balance, 25000)};
        Event good_event_1 = Event("Груз ананасов доставлен по назначению. В казну поступило 25000 ролодов. Исполнители, к сожалению, попали в аварию на горной дороге и упали с обрыва. Их автомобиль повреждений не получил и уже возвращён в гараж.\n", good_params_1);
        
        std::vector<std::pair<ChangingField, int>> bad_params_1 = {std::pair<ChangingField, int>(ChangingField::balance, -20000)};
        Event bad_event_1 = Event("Груз был перехвачен наркополицейскими в пункте назначения. К счастью, с вами это дело связать не удалось, так что политических последствий не будет. однако, некоторым людям пришлось заплатить: кому за хлопоты, кому за молчание. В итоге, вы потеряли 20000 ролодов.\n", bad_params_1);
        
        Quiz quiz_1 = Quiz("Нашим силовикам удалось перехватить огромную партию наркотиков. По закону, её нужно уничтожить, но есть возможность продать её в недружественные страны через подставных лиц. Уничтожить (1) или продать (2)?\n",
                           "Верное решение. Репутация страны на международной арене дороже любых денег, да и посредники были слишком мутные.\n",
                           good_event_1, bad_event_1);
        this->quizes.push_back(quiz_1);
        
        std::vector<std::pair<ChangingField, int>> good_params_2 = {std::pair<ChangingField, int>(ChangingField::balance, 30000)};
        Event good_event_2 = Event("Служба внешней разведки доложила, что господин бывший президент с семьёй был успешно вывезен с территории сопредельного государства. Операция была замаскирована под похищение конкурирующей партией, так что к нам вопросов нет.\n", good_params_2);
        
        std::vector<std::pair<ChangingField, int>> bad_params_2 = {std::pair<ChangingField, int>(ChangingField::balance, -20000)};
        Event bad_event_2 = Event("Служба внешней разведки доложила о провале операции. Судно, на котором перевозили бывшего президента, было перехвачено войсками нового правительства. Поскольку инцидент произошёл в нейтральных водах, а судно было под чужим флагом, нам не оставалось ничего другого, как сделать вид, что мы не имеем к операции никакого отношения. Общие потери по итогам предприятия составили 20000 ролодов.\n", bad_params_2);
                                  
        Quiz quiz_2 = Quiz("Свергнутый диктатор соседней страны просит убежища. Обещает заплатить 40000 ролодов. Отказаться (1) или предоставить (2)?\n",
                           "Взвешенное решение. В нашей демократической республике нет места таким негодяям. Тем более, если они оценивают свою жизнь в такую жалкую сумму.\n",
                           good_event_2, bad_event_2);
        this->quizes.push_back(quiz_2);
        
        std::vector<std::pair<ChangingField, int>> good_params_3 = {std::pair<ChangingField, int>(ChangingField::balance, 10000)};
        Event good_event_3 = Event("Открытие первой клиники прошло успешно. На территорию уже доставили первых пациентов на вертолётах без опознавательных знаков. Откат... Я хотел сказать, оплата за разрешение, поступили в казну.\n", good_params_3);
                                  
        std::vector<std::pair<ChangingField, int>> bad_params_3 = {std::pair<ChangingField, int>(ChangingField::balance, -5000)};
                                  bad_params_3.push_back(std::pair<ChangingField, int>(ChangingField::countryman_by_koef_per_countrymen, -5));
        Event bad_event_3 = Event("Это оказались не клиники, а вирусологические лаборатории, и вчера из них по недосмотру сбежала стая мышей, заражённых штаммом опасного вируса. К счастью, далеко им сбежать не удалось, но огонь уничтожил имущества в окрестных деревнях на 5000 ролодов, так же в огне погибли все заражённые, и, на всякий случай, то есть, в следствии несчастного случая, все члены их семей. Убыль населения составила 5%.\n", bad_params_3);
                                  
        Quiz quiz_3 = Quiz("Владельцы сети известных клиник хотят открыть в нашей стране несколько филиалов, за что предлагают выплату в 10000 ролодов и бесплатное лечение для вас и членов вашей семьи. Отказать (1) или разрешить (2)?\n",
                           "Похвальная осторожность. Незачем создавать конкуренцию отечественным светилам медицины. Кстати, я говорил, что мой старший сын недавно вернулся из Швейцарии, где блестяще закончил обучение на врача-герудотерапевта? Он вполне подойдёт на роль вашего семейного доктора.\n",
                           good_event_3, bad_event_3);
        this->quizes.push_back(quiz_3);
        
        std::vector<std::pair<ChangingField, int>> good_params_4 = {std::pair<ChangingField, int>(ChangingField::balance, 20000)};
        Event good_event_4 = Event("Феноменально!  Всю партию изделий смели за неделю. Ещё бы, флагманские характеристики  по очень привлекательной цене. Без экономии, разумеется, не обошлось, так что проработают аппараты максимум полгода. Хорошо, что в прошлом году мы не успели принять закон о защите прав потребителя. Так что о возвратах беспокоится не стоит. Итоговая прибыль 20000 ролодов.\n", good_params_4);
                                  
        std::vector<std::pair<ChangingField, int>> bad_params_4 = {std::pair<ChangingField, int>(ChangingField::balance, -10000)};
        Event bad_event_4 = Event("Поделка этих болванов оказалась никому не нужна!  В итоге, они разорились, а вы потеряли 10000 ролодов.\n", bad_params_4);
                                  
        Quiz quiz_4 = Quiz("Перспективная компания ищет средства на разработку революционного продукта. Отказаться (1) или предоставить финансирование (2)?\n",
                           "Резонно. Как говорится, лучше колибри в руках, чем фламинго в небе.\n",
                           good_event_4, bad_event_4);
        this->quizes.push_back(quiz_4);
        
        std::vector<std::pair<ChangingField, int>> good_params_5 = {std::pair<ChangingField, int>(ChangingField::balance, 30000)};
        Event good_event_5 = Event("Господин премьер-министр, они вернулись! Золота, правда, не нашли, зато наткнулись на огромный кусок амбры. Ваша доля, в смысле, отчисления в казну, составили 30000 ролодов.\n", good_params_5);
                                  
        std::vector<std::pair<ChangingField, int>> bad_params_5 = {std::pair<ChangingField, int>(ChangingField::balance, -20000)};
        Event bad_event_5 = Event("Господин премьер-министр, эти ротозеи не только не нашли сокровищ, но и умудрились угробить два корабля из трёх. Ваши потери, с учётом страховки, составили 20000 ролодов.\n", bad_params_5);
                                  
        Quiz quiz_5 = Quiz("Родственник одного из ваших приближённых утверждает, что ему по наследству досталась карта, на которой отмечено примерное местоположение затонувшего много лет назад корабля, перевозившего золото. Ему требуются средства на организацию экспедиции. В случае успеха он обещает передать 40% в пользу государства. Откажете (1) или профинансируете его авантюру (2)?\n",
                           "Разумно! Какой идиот в наше время поверит в пиратские сокровища? Кстати, не могли бы вы предоставить мне отпуск? Я давно обещал жене морской круиз.\n",
                           good_event_5, bad_event_5);
        this->quizes.push_back(quiz_5);
        
        std::vector<std::pair<ChangingField, int>> good_params_6 = {std::pair<ChangingField, int>(ChangingField::harvest_percentage, 100)};
        Event good_event_6 = Event("Хорошие новости! Согласно анализу почвы, новое удобрение сработало. Говорить о результатах пока, конечно, рано, но, по словам специалистов, в этом году можно ожидать двукратного прироста урожая.", good_params_6);
        
        std::vector<std::pair<ChangingField, int>> bad_params_6 = {std::pair<ChangingField, int>(ChangingField::harvest_percentage, -50)};
        Event bad_event_6 = Event("Эти горе-учёные опять что-то напутали. По словам приглашённых независимых специалистов, плодородность почвы упала в 2 раза, так что в текущий год нас ждёт неурожай.", bad_params_6);
        Quiz quiz_6 = Quiz("Учёный совет просит разрешить проведение испытаний нового удобрения. Откажете (1) или разрешите (2)?\n",
                           "Правильно! Всё зло от химии!\n",
                           good_event_6, bad_event_6);
        this->quizes.push_back(quiz_6);
        
        std::vector<std::pair<ChangingField, int>> good_params_7 = {std::pair<ChangingField, int>(ChangingField::cost_of_life_percentage, -25)};
        Event good_event_7 = Event("Поздравляю, господин премьер-министр! Испытательные прогоны увенчались полным успехом, а регулярные рейсы начнутся с этого года.", good_params_7);
        
        std::vector<std::pair<ChangingField, int>> bad_params_7 = {std::pair<ChangingField, int>(ChangingField::balance, -10000)};
        Event bad_event_7 = Event("Проект провалился. Почва в месте прокладки оказалась неподходящей. Потери казны составили 10000 ролодов.", bad_params_7);
        
        Quiz quiz_7 = Quiz("Министерство транспорта предложило проект прокладки железной дороги по части территории острова. В случае успеха мы сможем снизить стоимость жизни граждан в будущем году на 25%. Отказать (1) или разрешить (2)?\n",
                           "Дальновидно. Консервативно настроенные граждане могут не одобрить такой инновации. Лошадки понадёжнее будут.\n",
                           good_event_7, bad_event_7);
        this->quizes.push_back(quiz_7);
        
        auto rd = std::random_device {};
        auto rng = std::default_random_engine { rd() };
        std::shuffle(std::begin(this->quizes), std::end(this->quizes), rng);
    }
    
//    Quiz get_random_quiz() {
//            int quizes_count = static_cast<int>(this->quizes.size());
//            int prob = 100 / quizes_count;
//            int choise = get_random_choise(quizes_count, prob);
//            Quiz choosen = this->quizes[choise];
//            this->quizes.erase(this->quizes.begin() + choise);
//            return choosen;
//    }
    
    Quiz pop_last_quiz() {
        // взять последнюю игру из (заранее перемешанного) списка и удалить ее из списка
        Quiz choosen = this->quizes.back();
        this->quizes.pop_back();
        this->quizes.insert(this->quizes.begin(), choosen);
        return choosen;
    }
};

class Events {
private:
    std::vector<Event> good_events;
    std::vector<Event> bad_events;
    
public:
    Events() {
        
        // положительные события
        
        std::vector<std::pair<ChangingField, int>> good_params_1 = {std::pair<ChangingField, int>(ChangingField::balance, 30000)};
        Event event_1 = Event("Министр внутренних дел доложил о поимке чиновника-взяточника. Негодяй отгрохал себе огромную виллу. После её конфискации бюджет пополнился на 30000 ролодов.\n", good_params_1);
        this->good_events.push_back(event_1);
        
        std::vector<std::pair<ChangingField, int>> good_params_2 = {std::pair<ChangingField, int>(ChangingField::balance, 30000)};
        Event event_2 = Event("Всемирный Банк развития выделил вашей стране кредит на сумму 30000 ролодов. Правда, под 150% годовых. Но учитывая срок в 16 лет и выплату в конце срока, это уже не ваши проблемы. Правда, о третьем премьерском сроке лучше забыть.\n", good_params_2);
        this->good_events.push_back(event_2);
        
        std::vector<std::pair<ChangingField, int>> good_params_3 = {std::pair<ChangingField, int>(ChangingField::countryman_next_year, 50)};
        Event event_3 = Event("У наших берегов потерпел крушение корабль. К счастью, большая часть пассажиров и членов экипажа выжила, так что население острова пополнилось ещё на 50 голодных ртов.\n", good_params_3);
        this->good_events.push_back(event_3);
        
        std::vector<std::pair<ChangingField, int>> good_params_4 = {std::pair<ChangingField, int>(ChangingField::balance, 10000)};
        Event event_4 = Event("На острове потерпел крушение воздушный шар. Поисковая группа уже обнаружила его в одном из лесов. К сожалению, никому из пассажиров и членов экипажа выжить не удалось. Предположительно, шар принадлежит одному из крупных промышленников, но поскольку шар сохранился плохо и однозначно опознан быть не может, мы не сочли возможным сообщать этому человеку  о находке, чтобы ненароком не расстроить. Размер добычи, то есть, я хотел сказать, общая стоимость найденного  имущества составила 10000 ролодов.\n", good_params_4);
        this->good_events.push_back(event_4);
        
        std::vector<std::pair<ChangingField, int>> good_params_5 = {std::pair<ChangingField, int>(ChangingField::tourism_percentage, 50)};
        Event event_5 = Event("Известный блогер-путешественник выпустил серию видеороликов о нашем острове. Из-за повышения интереса со стороны приезжих, доход от туризма в следующем году вырастет на 50%. Полагаю, уже можно выпускать этого бедолагу из подвала.\n", good_params_5);
        this->good_events.push_back(event_5);
        
        std::vector<std::pair<ChangingField, int>> good_params_6 = {std::pair<ChangingField, int>(ChangingField::balance, 25000)};
        Event event_6 = Event("Биологи обнаружили на нашем острове ранее неизвестную бактерию. За право на проведение исследований они заплатили 25000 ролодов. Надеюсь, она не окажется болезнетворной.\n", good_params_6);
        this->good_events.push_back(event_6);
        
        std::vector<std::pair<ChangingField, int>> good_params_7 = {std::pair<ChangingField, int>(ChangingField::balance, 20000)};
        Event event_7 = Event("Наш живописный остров использовали в качестве декорации для съёмок фильма о дикарях-каннибаллах. Правда, к  широкому прокату его не допустили из-за обилия жестоких и вульгарных сцен. Но как бы то ни было, за разрешение на съёмки киношники заплатили 20000 ролодов.\n", good_params_7);
        this->good_events.push_back(event_7);
        
        std::vector<std::pair<ChangingField, int>> good_params_8 = {std::pair<ChangingField, int>(ChangingField::tourism_percentage, 25)};
        Event event_8 = Event("Завершилась реставрация одного из лесных заповедников, начатая ещё 20 лет назад вашим предшественником. Из-за всплеска интереса со стороны любителей экзотических животных, в будущем году мы ожидаем увеличения дохода от туризма на 25%. Полагаю, животные тоже будут рады, разнообразить рацион: это всегда полезно.\n",
                              good_params_8);
        this->good_events.push_back(event_8);
        
        std::vector<std::pair<ChangingField, int>> good_params_9 = {std::pair<ChangingField, int>(ChangingField::balance, 15000)};
        Event event_9 = Event("В одной из ваших загородных резиденций произошёл пожар. К счастью, она была застрахована. Вы решили, что ещё одна резиденция вам не слишком нужна, так что сумма страховки в размере 15000 ролодов поступила в государственную казну.\n", good_params_9);
        this->good_events.push_back(event_9);
        
        std::vector<std::pair<ChangingField, int>> good_params_10 = {std::pair<ChangingField, int>(ChangingField::balance, 15000)};
        Event event_10 = Event("Полиция пресекла деятельность банды, вывозившей с острова ценных животных. По решению суда у преступников конфисковано имущества на 15000 ролодов.\n", good_params_10);
        this->good_events.push_back(event_10);
        
        // отрицательные события
        
        std::vector<std::pair<ChangingField, int>> bad_params_1 = {std::pair<ChangingField, int>(ChangingField::balance, -9000)};
        Event bad_event_1 = Event("В ваш день рождения был объявлен всенародный праздник с дармовой выпивкой. Жители были столь усердны в поднятии тостов за ваше здоровье, что уничтожили алкогольных напитков на 5000 ролодов. Кроме того, некоторые особо приближённые гости столь бурно изливали потоки ликования, что на отмывку и ремонт полов в резиденции ушло ещё 4000.\n", bad_params_1);
        this->bad_events.push_back(bad_event_1);
        
        std::vector<std::pair<ChangingField, int>> bad_params_2 = {std::pair<ChangingField, int>(ChangingField::balance, -3500)};
        Event bad_event_2 = Event("Во время инспекционной поездки в общественные бани вы получили серьёзный ожог. На лечение ушло 3500 ролодов, с учётом доплаты за срочность: не так легко найти неразговорчивого доктора в 3 часа ночи.\n", bad_params_2);
        this->bad_events.push_back(bad_event_2);
        
        std::vector<std::pair<ChangingField, int>> bad_params_3 = {std::pair<ChangingField, int>(ChangingField::balance, -2000)};
        Event bad_event_3 = Event("Природная аномалия. На наши поля пролился дождь из лягушек. Покупка специально обученных аистов обошлась в 2000 ролодов. От лягушек избавились, теперь нужно придумать, куда девать аистов.\n", bad_params_3);
        this->bad_events.push_back(bad_event_3);
        
        std::vector<std::pair<ChangingField, int>> bad_params_4 = {std::pair<ChangingField, int>(ChangingField::balance, -3500)};
        Event bad_event_4 = Event("Из-за отмены любимого народом сериала граждане устроили недельную забастовку. Убытки составили 3500 ролодов.\n", bad_params_4);
        this->bad_events.push_back(bad_event_4);
        
        std::vector<std::pair<ChangingField, int>> bad_params_5 = {std::pair<ChangingField, int>(ChangingField::balance_by_koef_per_countryman, -20)};

        Event bad_event_5 = Event("Согласно докладу учёного совета, повышенная солнечная активность привела к усилению головных болей у населения. Вышеназванный совет уже  разработал специальные шапочки из сверхсекретного сплава для нейтрализации влияния излучения . Их производство и закупка обошлись в 20 ролодов на каждого жителя.\n", bad_params_5);
        this->bad_events.push_back(bad_event_5);
        
        std::vector<std::pair<ChangingField, int>> bad_params_6 = {std::pair<ChangingField, int>(ChangingField::balance, -13000)};
        Event bad_event_6 = Event("Знаменитая певица привезла из зарубежных гастролей выводок каких-то мелких милых зверушек. После публикации в газете  фотографий красавицы в обнимку с одной из них, наши падкие на экзотику граждане немедленно захотели себе таких же. Оказалось, что зверьки способны крайне быстро размножаться, однако радость от этого факта была недолгой. Эти твари жрут всё, до чего могут добраться. В итоге, мы потеряли продовольствия на 3000 ролодов, и ещё 10000 ушло на мероприятия по борьбе с этими вредителями. Самое обидное, мы даже не можем взыскать с этой певички ущерб. Как только она поняла, к чему идёт дело, тут же сбежала из страны.\n", bad_params_6);
        this->bad_events.push_back(bad_event_6);
        
        std::vector<std::pair<ChangingField, int>> bad_params_7 = {std::pair<ChangingField, int>(ChangingField::balance, -900)};
        Event bad_event_7 = Event("Король соседней страны, с которым вы когда-то вместе учились, прислал вам в подарок огромную маринованную рыбину. К сожалению, из-за того, что наша почтовая система работает не очень хорошо, деликатес пролежал на жаре две недели. Несмотря на предупреждения повара, вы всё же решили продегустировать угощение. Блюдо оказалось необычайно вкусным, а   Лечение отравления  обошлось вам всего  в 900 ролодов. Так что, может быть, оно того стоило.\n", bad_params_7);
        this->bad_events.push_back(bad_event_7);
        
        std::vector<std::pair<ChangingField, int>> bad_params_8 = {std::pair<ChangingField, int>(ChangingField::balance, -20000)};
        Event bad_event_8 = Event("Любовница  министра финансов нашего государства поступила в обучение к некоему бизнес-гуру. После того, как выяснилось, что уроки мэтр предпочитает давать исключительно в постели, министр заявил на него в полицию. К сожалению, пройдоха успел сбежать через окно спальни. Разыскные мероприятия результатов не дали. По итогам проверки стало известно, что на \"обучение\" было потрачено 20000 ролодов государственных средств. Узнав об этом, министр финансов попытался задушить любовницу, а затем скрылся тем же путём, что и ранее упомянутый субъект.\n", bad_params_8);
        this->bad_events.push_back(bad_event_8);
        
        std::vector<std::pair<ChangingField, int>> bad_params_9 = {std::pair<ChangingField, int>(ChangingField::balance, -10000)};
        bad_params_9.push_back(std::pair<ChangingField, int>(ChangingField::countryman, -30));
        Event bad_event_9 = Event("При проведении горных работ во время строительства новой шахты, рабочие докопались до древнего чудовища. Реликтовый монстр не оценил дружелюбия наших граждан, ну или просто успел за столько лет беспробудного сна проголодаться. В результате неудачного контакта мы потеряли 30 рабочих и несколько единиц тяжёлой техники общей стоимостью 10000 ролодов. Местонахождение монстра в данный момент неизвестно.\n", bad_params_9);
        this->bad_events.push_back(bad_event_9);
        
        std::vector<std::pair<ChangingField, int>> bad_params_10 = {std::pair<ChangingField, int>(ChangingField::balance, -10000)};
        Event bad_event_10 = Event("На одно из ваших полей упал метеорит. Жертв и разрушений нет. Однако, мы терпим убытки, поскольку  жители вместо того, чтобы работать, глазеют на воронку. Кроме того, несколько несознательных граждан получили увечья, вступив в драку за обладание осколками небесного тела. Ущерб составил 10000 роллодов.\n", bad_params_10);
        this->bad_events.push_back(bad_event_10);
        
        auto rd = std::random_device {};
        auto rng = std::default_random_engine { rd() };
        std::shuffle(std::begin(this->good_events), std::end(this->good_events), rng);
        std::shuffle(std::begin(this->bad_events), std::end(this->bad_events), rng);
    }
    
//    Event get_random_event(EventType event_type) {
//        if (event_type == EventType::good) {
//            int events_count = static_cast<int>(this->good_events.size());
//            int prob = 100 / events_count;
//            int choise = get_random_choise(events_count, prob);
//            Event choosen = this->good_events[choise];
//            this->good_events.erase(this->good_events.begin() + choise);
//            return choosen;
//        } else { // if (event_type == EventType::bad) {
//            int events_count = static_cast<int>(this->bad_events.size());
//            int prob = 100 / events_count;
//            int choise = get_random_choise(events_count, prob);
//            Event choosen =  this->bad_events[choise];
//            this->bad_events.erase(this->bad_events.begin() + choise);
//            return choosen;
//        }
//    }
    
    Event popLastEvent(EventType event_type) {
        // взять последнее событие из (заранее перемешанного) списка и удалить его из списка
        if (event_type == EventType::good) {
            Event choosen = this->good_events.back();
            this->good_events.pop_back();
            this->good_events.insert(this->good_events.begin(), choosen);
            return choosen;
        } else {
            Event choosen = this->bad_events.back();
            this->bad_events.pop_back();
            this->bad_events.insert(this->bad_events.begin(), choosen);
            return choosen;
        }
    }
};


enum class monthType {
    GOOD,
    BAD,
    QUIZ,
    EMPTY
};

class YearEvents {
private:
    short good_events_count = 1;
    short bad_events_count = 1;
    short quizes_count = 1;
    short no_events_count = 0;
    
public:
    std::vector<monthType> monthes;
    
    YearEvents() {
//        good_events_count = get_random_choise(3, 33);
//        bad_events_count = get_random_choise(3, 33);
//        quizes_count = get_random_choise(2, 50);
        no_events_count = 12 - good_events_count - bad_events_count - quizes_count;
        
        for (size_t i = 0; i < good_events_count; ++i) { monthes.push_back(monthType::GOOD); }
        for (size_t i = 0; i < bad_events_count; ++i) { monthes.push_back(monthType::BAD); }
        for (size_t i = 0; i < quizes_count; ++i) { monthes.push_back(monthType::QUIZ); }
        for (size_t i = 0; i < no_events_count; ++i) { monthes.push_back(monthType::EMPTY); }
        
        auto rd = std::random_device {};
        auto rng = std::default_random_engine { rd() };
        std::shuffle(std::begin(this->monthes), std::end(this->monthes), rng);
    }
};
