#include <gtest/gtest.h>
#include <functional>
#include <list>
#include <memory>

#include <COLA.hh>

#include "lib/condition/builders/group.h"
#include "lib/config/registry.h"
#include "lib/config/stage.h"
#include "lib/builders/module.h"
#include "lib/builders/xml.h"
#include "lib/model/iface.h"
#include "lib/util/util.h"

using namespace xml_impl;
using namespace config_builder;

namespace {
    pugi::xml_document ParseConfig(const std::string& config) {
        pugi::xml_document doc;

        pugi::xml_parse_result result = doc.load_string(config.c_str());
        if (!result) {
            throw std::runtime_error(std::string("document error: ") + result.description());
        }
        return doc;
    }

    void ReadConfig(const std::string& config, ModuleData& data) {
        auto doc = ParseConfig(config);
        const auto& node = doc.first_child();
        auto visiter = ModuleVisiter(data);
        TraverseModuleTree(node, visiter);
    }

    Module BuildModule(const std::string& config) {
        auto doc = ParseConfig(config);
        return ModuleBuilder().Build(doc.first_child());
    }

    REGISTER_CONDITION(true, [](const cola::Particle&) -> bool { return true; });
    REGISTER_CONDITION(false, [](const cola::Particle&) -> bool { return false; });

    static int CALL_COUNTER = 0;

    class LoggingConditionBuilder : public IConditionBuilder {
    public:
        Condition Build(const Context&) {
            return [&](const cola::Particle&) -> bool {
                CALL_COUNTER += 1;
                return true;
            };
        }
    };
    REGISTER_CONDITION_BUILDER(logger, new LoggingConditionBuilder());

    REGISTER_CONDITION(logger, [](const cola::Particle&) {
        CALL_COUNTER += 1;
        return true;
    })

    class AppendingModel : public IModel {
    public:
        AppendingModel(const cola::Particle& particle) : new_particle_(particle) {}

        cola::EventParticles BreakItUp(const cola::Particle& particle) override {
            return {particle, new_particle_};
        }

    private:
        cola::Particle new_particle_;
    };
    REGISTER_MODEL(append, new AppendingModel({}));
}  // anonymous namespace

TEST(TestConditionBuilder, TestORGroupBuilder) {
    auto config = R"(
    <?xml version='1.0'?>
    <condition name='or'>
        <or>
            <condition source='true'/>
            <condition source='false'/>
        </or>
    </condition>
    )";
    ModuleData data;
    ASSERT_NO_THROW(ReadConfig(config, data))  << "config is valid";

    auto or_condition = data.conditions.Find("or");
    ASSERT_TRUE(or_condition.has_value()) << "must contain 'or' condition";
    EXPECT_TRUE(or_condition.value().get()(cola::Particle{})) << "has only true child";
}

TEST(TestConditionBuilder, TestANDGroupBuilder) {
    auto config = R"(
    <?xml version='1.0'?>
    <condition name='and'>
        <and>
            <condition source='true'/>
            <condition source='false'/>
        </and>
    </condition>
    )";
    ModuleData data;
    ASSERT_NO_THROW(ReadConfig(config, data))  << "config is valid";

    auto and_condition = data.conditions.Find("and");
    ASSERT_TRUE(and_condition.has_value()) << "must contain 'and' condition";
    EXPECT_FALSE(and_condition.value().get()(cola::Particle{})) << "has only false child";
}

TEST(TestConditionBuilder, TestCompareBuiler) {
    auto config = R"(
    <?xml version='1.0'?>
    <condition name='compare'>
        <eq property='A'>11</eq>
    </condition>
    )";
    ModuleData data;
    ASSERT_NO_THROW(ReadConfig(config, data))  << "config is valid";

    auto cmp_condition = data.conditions.Find("compare");

    // equal
    cola::Particle particle;
    particle.pdgCode = cola::AZToPdg({11, 5});
    EXPECT_TRUE(cmp_condition.value().get()(particle)) << "'11' == '11'";

    // different
    particle.pdgCode = cola::AZToPdg({5, 5});
    EXPECT_FALSE(cmp_condition.value().get()(particle)) << "'5' != '11'";
}

TEST(TestConditionBuilder, TestConditionSource) {
    {
        auto config = R"(
        <?xml version='1.0'?>
        <condition source='unknown_source'/>
        )";
        ModuleData data;
        ASSERT_THROW(ReadConfig(config, data), std::runtime_error) << "config is invalid, missing source";
    }

    auto config = R"(
    <?xml version='1.0'?>
    <condition name='test'>
        <condition source='logger'/>
    </condition>
    )";
    CALL_COUNTER = 0;

    ModuleData data;
    ASSERT_NO_THROW(ReadConfig(config, data)) << "config is valid";

    auto test_condition = data.conditions.Find("test");
    ASSERT_TRUE(test_condition.has_value()) << "config specifies 'test'";
    ASSERT_TRUE(test_condition.value().get()(cola::Particle{})) << "'logger' return true";
    ASSERT_EQ(CALL_COUNTER, 1) << "'logger' increments counter";
}

TEST(TestConditionBuilder, TestConditionLink) {
    {
        auto config = R"(
        <?xml version='1.0'?>
        <condition link='base'/>
        )";
        ModuleData data;
        ASSERT_THROW(ReadConfig(config, data), std::runtime_error) << "config is invalid, missing link";
    }

    auto config = R"(
    <?xml version='1.0'?>
    <module>
        <condition name='base'>
            <condition source='logger'/>
        </condition>
        <condition name='test'>
            <condition link='base'/>
        </condition>
    </module>
    )";
    CALL_COUNTER = 0;

    ModuleData data;
    ASSERT_NO_THROW(ReadConfig(config, data)) << "config is valid";

    auto base_condition = data.conditions.Find("base");
    ASSERT_TRUE(base_condition.has_value()) << "config specifies 'base'";
    ASSERT_TRUE(base_condition.value().get()(cola::Particle{})) << "'logger' return true";
    ASSERT_EQ(CALL_COUNTER, 1) << "'logger' increments counter";

    auto test_condition = data.conditions.Find("test");
    ASSERT_TRUE(test_condition.has_value()) << "config specifies 'test'";
    EXPECT_TRUE(test_condition.value().get()(cola::Particle{})) << "'test' mimics 'logger'";
    EXPECT_EQ(CALL_COUNTER, 2) << "'test' is 'logger' clone";
}

TEST(TestConditionBuilder, TestConditionBuild) {
    {
        auto config = R"(
        <?xml version='1.0'?>
        <condition build='unknown_build'/>
        )";
        ModuleData data;
        ASSERT_THROW(ReadConfig(config, data), std::runtime_error) << "config is invalid, missing build";
    }

    auto config = R"(
    <?xml version='1.0'?>
    <condition name='test'>
        <condition build='logger'/>
    </condition>
    )";
    CALL_COUNTER = 0;

    ModuleData data;
    ASSERT_NO_THROW(ReadConfig(config, data)) << "config is valid";

    auto test_condition = data.conditions.Find("test");
    ASSERT_TRUE(test_condition.has_value()) << "config specifies 'test'";
    ASSERT_TRUE(test_condition.value().get()(cola::Particle{})) << "'logger' return true";
    EXPECT_EQ(CALL_COUNTER, 1) << "'logger' increments counter";
}

TEST(TestConditionBuilder, TestConditionValidation) {
    {
        auto config = R"(
        <?xml version='1.0'?>
        <condition/>
        )";
        ModuleData data;
        ASSERT_THROW(ReadConfig(config, data), std::out_of_range) << "config is invalid, missing name";
    }

    {
        auto config = R"(
        <?xml version='1.0'?>
        <condition name='name'>
            <module/>
        </condition>
        )";
        ModuleData data;
        ASSERT_THROW(ReadConfig(config, data), std::runtime_error) << "config is invalid, bad condition";
    }

    {
        auto config = R"(
        <?xml version='1.0'?>
        <condition name='name'>
            <and/>
            <and/>
        </condition>
        )";
        ModuleData data;
        ASSERT_THROW(ReadConfig(config, data), std::runtime_error) << "config is invalid, multiple conditions";
    }

    {
        auto config = R"(
        <?xml version='1.0'?>
        <module>
            <condition name='name'>
                <and/>
            </condition>
            <condition name='name'>
                <and/>
            </condition>
        </module>
        )";
        ModuleData data;
        ASSERT_THROW(ReadConfig(config, data), std::runtime_error) << "config is invalid, condition is redefined";
    }
}

TEST(TestStage, TestStage) {
    std::list<Stage> stages;
    stages.push_back(BaseStage());
    auto& stage = std::get<BaseStage>(stages.back());
    // test no drain
    ASSERT_THROW(stage.Propogate(cola::Particle{}), std::runtime_error) << "no next stages";

    stages.push_back(BaseStage());
    auto& optional_stage = std::get<BaseStage>(stages.back());
    stage.stages.emplace_back(stages.back());
    optional_stage.condition = [](const cola::Particle&) {
        CALL_COUNTER += 1;
        return false;
    };
    stages.push_back(DrainStage());
    auto& optional_drain = std::get<DrainStage>(stages.back());
    optional_stage.stages.emplace_back(stages.back());

    stages.push_back(DrainStage());
    auto& drain = std::get<DrainStage>(stages.back());
    stage.stages.emplace_back(stages.back());

    // test drain
    CALL_COUNTER = 0;
    ASSERT_NO_THROW(stage.Propogate(cola::Particle{})) << "has default drain";
    EXPECT_EQ(CALL_COUNTER, 1) << "condition must be checked";
    EXPECT_EQ(drain.result.size(), 1) << "initial particle goes to drain";
    drain.result.clear();

    // test stage spliting
    optional_stage.condition = [](const cola::Particle&) {
        CALL_COUNTER += 1;
        return true;
    };
    ASSERT_NO_THROW(stage.Propogate(cola::Particle{})) << "all stages has default drain";
    EXPECT_EQ(CALL_COUNTER, 2) << "condition must be checked";
    EXPECT_EQ(drain.result.size(), 0) << "initial particle goes to optional drain";
    EXPECT_EQ(optional_drain.result.size(), 1) << "initial particle goes to optional drain";
    drain.result.clear();
    optional_drain.result.clear();

    // test model call
    cola::Particle initial_particle;
    initial_particle.pdgCode = 0;
    cola::Particle append_particle;
    append_particle.pdgCode = 1;
    auto model = std::unique_ptr<IModel>(new AppendingModel(append_particle));
    stage.model = *model;
    optional_stage.condition = [&](const cola::Particle& particle) {
        CALL_COUNTER += 1;
        return particle.pdgCode == append_particle.pdgCode;
    };
    ASSERT_NO_THROW(stage.Propogate(initial_particle)) << "all stages has default drain";
    EXPECT_EQ(CALL_COUNTER, 4) << "condition must be checked twice";
    ASSERT_EQ(drain.result.size(), 1) << "initial particle goes to initial drain";
    EXPECT_EQ(drain.result[0].pdgCode, initial_particle.pdgCode) << "initial_particle goes to base drain";
    ASSERT_EQ(optional_drain.result.size(), 1) << "appended particle goes to optional drain";
    EXPECT_EQ(optional_drain.result[0].pdgCode, append_particle.pdgCode) << "append_particle goes to optional drain";
}

TEST(TestStageBuilder, TestStageLink) {
    auto config = R"(
    <?xml version='1.0'?>
    <module>
        <stage name='base'>
            <condition source='logger'/>
            <stage link='drain'/>
        </stage>
        <stage name='test'>
            <stage link='base'/>
        </stage>
    </module>
    )";
    CALL_COUNTER = 0;

    ModuleData data;
    data.stages.Register("drain", DrainStage());
    ASSERT_NO_THROW(ReadConfig(config, data)) << "config is valid";

    auto drain_stage = data.stages.Find("drain");
    ASSERT_TRUE(drain_stage.has_value());
    ASSERT_TRUE(std::holds_alternative<DrainStage>(drain_stage.value().get()));
    auto& drain = std::get<DrainStage>(drain_stage.value().get());

    {
        auto stage = data.stages.Find("base");
        ASSERT_TRUE(stage.has_value()) << "config specifies 'base'";
        ASSERT_TRUE(std::holds_alternative<BaseStage>(stage.value().get())) << "config defines BaseStage";
        auto& base_stage = std::get<BaseStage>(stage.value().get());

        bool is_processed = false;
        ASSERT_NO_THROW(is_processed = base_stage.Propogate(cola::Particle{}));
        EXPECT_TRUE(is_processed);
        EXPECT_EQ(CALL_COUNTER, 1) << "'logger' increments counter";
        EXPECT_EQ(drain.result.size(), 1);
        drain.result.clear();
    }

    {
        auto stage = data.stages.Find("test");
        ASSERT_TRUE(stage.has_value()) << "config specifies 'test'";
        ASSERT_TRUE(std::holds_alternative<BaseStage>(stage.value().get())) << "config defines BaseStage";
        auto& test_stage = std::get<BaseStage>(stage.value().get());

        bool is_processed = false;
        ASSERT_NO_THROW(is_processed = test_stage.Propogate(cola::Particle{}));
        EXPECT_TRUE(is_processed);
        EXPECT_EQ(CALL_COUNTER, 2) << "'test' is 'base' clone";
        EXPECT_EQ(drain.result.size(), 1);
        drain.result.clear();
    }
}

TEST(TestStageBuilder, TestStageBuilder) {
    {
        auto config = R"(
        <?xml version='1.0'?>
        <stage/>
        )";
        ModuleData data;
        ASSERT_THROW(ReadConfig(config, data), std::out_of_range) << "config is invalid, missing name";
    }

    {
        auto config = R"(
        <?xml version='1.0'?>
        <module>
            <stage name='name'/>
            <stage name='name'/>
        </module>
        )";
        ModuleData data;
        ASSERT_THROW(ReadConfig(config, data), std::runtime_error) << "config is invalid, duplicate name";
    }

    auto config = R"(
    <?xml version='1.0'?>
    <stage name='base'>
        <model source='append'/>
        <condition source='logger'/>
        <stage link='drain'/>
    </stage>
    )";
    CALL_COUNTER = 0;

    ModuleData data;
    data.stages.Register("drain", DrainStage());
    ASSERT_NO_THROW(ReadConfig(config, data)) << "config is valid";

    auto drain_stage = data.stages.Find("drain");
    ASSERT_TRUE(drain_stage.has_value());
    ASSERT_TRUE(std::holds_alternative<DrainStage>(drain_stage.value().get()));
    auto& drain = std::get<DrainStage>(drain_stage.value().get());

    {
        auto stage = data.stages.Find("base");
        ASSERT_TRUE(stage.has_value()) << "config specifies 'base'";
        ASSERT_TRUE(std::holds_alternative<BaseStage>(stage.value().get())) << "config defines BaseStage";
        auto& base_stage = std::get<BaseStage>(stage.value().get());

        auto is_processed = false;
        cola::Particle initial_particle;
        initial_particle.pdgCode = 1;
        ASSERT_NO_THROW(is_processed = base_stage.Propogate(initial_particle)) << "'base' is valid";
        EXPECT_TRUE(is_processed);
        EXPECT_EQ(CALL_COUNTER, 1) << "'logger' is called";
        EXPECT_EQ(drain.result.size(), 2);
        EXPECT_EQ(drain.result.front().pdgCode, initial_particle.pdgCode);
    }
}

TEST(TestModuleBuilder, TestModuleBuilder) {
    {
        auto config = R"(
        <?xml version='1.0'?>
        <module initial_stage='in'>
            <stage name='in'>
                <stage link='drain'/>
            </stage>
        </module>
        )";
        ASSERT_THROW(BuildModule(config), std::out_of_range) << "module has missing drain";
    }

    {
        auto config = R"(
        <?xml version='1.0'?>
        <module drain_stage='out'>
            <stage name='in'>
                <stage link='drain'/>
            </stage>
        </module>
        )";
        ASSERT_THROW(BuildModule(config), std::out_of_range) << "module has missing source";
    }

    {
        auto config = R"(
        <?xml version='1.0'?>
        <module initial_stage='in' drain_stage='out'>
        </module>
        )";
        ASSERT_THROW(BuildModule(config), std::bad_optional_access) << "module missing source definition";
    }

    auto config = R"(
    <?xml version='1.0'?>
    <module initial_stage='in' drain_stage='out'>
        <stage name='other'>
            <model source='append'/>
            <condition source='logger'/>
            <stage link='out'/>
        </stage>
        <stage name='in'>
            <stage link='other'/>
            <stage link='out'/>
        </stage>
    </module>
    )";

    CALL_COUNTER = 0;
    std::unique_ptr<Module> module_ptr;
    ASSERT_NO_THROW(module_ptr = std::make_unique<Module>(BuildModule(config))) << "config is valid";
    auto& module = *module_ptr;

    cola::Particle particle;
    particle.pdgCode = 1;

    {
        cola::EventParticles result;
        ASSERT_NO_THROW(result = module.BreakItUp(particle));
        ASSERT_EQ(result.size(), 2);
        EXPECT_EQ(CALL_COUNTER, 1) << "'logger' is called";
        EXPECT_EQ(result.front().pdgCode, particle.pdgCode);
    }

    // test for reset
    {
        cola::EventParticles result;
        ASSERT_NO_THROW(result = module.BreakItUp(particle));
        ASSERT_EQ(result.size(), 2);
        EXPECT_EQ(CALL_COUNTER, 2) << "'logger' is called";
        EXPECT_EQ(result.front().pdgCode, particle.pdgCode);
    }
}

class ParameterConditionChecker : public IConditionBuilder {
public:
    ParameterConditionChecker(Params& params) : params_(params) {}

    Condition Build(const Context& context) {
        params_ = context.parameters;
        return [](const cola::Particle&) -> bool {
            return true;
        };
    }

private:
    Params& params_;
};

TEST(TestParameters, TestParameters) {
    auto config = R"(
    <?xml version='1.0'?>
    <condition build='parameters_build'>
        <param name='name'>value</param>
        <param name='name1'>value1</param>
        <param name='name2'>value2</param>
    </condition>
    )";
    ModuleData data;

    Params params;
    ConditionBuilderRegistry::Register("parameters_build", new ParameterConditionChecker(params));
    DEFER {
        ConditionBuilderRegistry::Unregister("parameters_build");
    };

    ASSERT_NO_THROW(ReadConfig(config, data)) << "config is valid";
    ASSERT_EQ(params.size(), 3);

    ASSERT_TRUE(params.contains("name"));
    EXPECT_EQ(params["name"], "value");

    ASSERT_TRUE(params.contains("name1"));
    EXPECT_EQ(params["name1"], "value1");

    ASSERT_TRUE(params.contains("name2"));
    EXPECT_EQ(params["name2"], "value2");
}
